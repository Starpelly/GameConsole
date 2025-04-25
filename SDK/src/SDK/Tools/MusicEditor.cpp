#include "MusicEditor.hpp"
#include "ui_musiceditor.h"

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <QFontDatabase>

struct TempoChange
{
    double seconds;
    double bpm;
};

static std::vector<TempoChange> GetTempoChanges(const smf::MidiFile& midi)
{
    std::vector<TempoChange> tempos;
    for (int t = 0; t < midi.getTrackCount(); ++t)
    {
        for (int i = 0; i < midi[t].size(); ++i)
        {
            auto& ev = midi[t][i];
            if (ev.isTempo()) {
                tempos.push_back({ ev.seconds, ev.getTempoBPM() });
            }
        }
    }

    std::sort(tempos.begin(), tempos.end(), [](auto& a, auto& b) {
        return a.seconds < b.seconds;
    });

    return tempos;
}

static double GetInitialTempo(smf::MidiFile& midi)
{
    for (int track = 0; track < midi.getTrackCount(); ++track)
    {
        for (int i = 0; i < midi[track].size(); ++i)
        {
            auto& ev = midi[track][i];

            if (ev.isTempo())
            {
                return ev.getTempoBPM();
            }
        }
    }
    return 120.0; // fallback default BPM
}

static std::string GetNoteName(int midiNote)
{
    static const char* noteNames[] = {
        "C", "C#", "D", "D#", "E", "F",
        "F#", "G", "G#", "A", "A#", "B"
    };

    int note = midiNote % 12;
    // int octave = midiNote / 12 - 1;  // MIDI octave -1 to 9
    int octave = midiNote / 12; // FL Studio-style

    return noteNames[note] + std::to_string(octave);
}

static bool IsBlackKey(int midiNote)
{
    int semitone = midiNote % 12;
    return semitone == 1 || semitone == 3 || semitone == 6 || semitone == 8 || semitone == 10;
}

static int GetOctave(int midiNote)
{
    return (midiNote / 12) - 1;  // Subtract 1 because MIDI note 0 = C–1
}

MusicEditor::MusicEditor(const QString& path, QWidget *parent)
    : QWidget(parent)
    , widget(new MusicWidget(this))
    , ui(new Ui::MusicEditor)
{
    ui->setupUi(this);

    data = new MusicData();

    Init(path);
}

MusicEditor::~MusicEditor()
{
    delete data;
    delete ui;
}

void MusicEditor::Init(const QString &path)
{
    // Music data
    {
        data->midifile.read(path.toStdString());
        data->midifile.doTimeAnalysis();

        // @Note: sometimes, getting the initial tempo will not work!
        // Pls fix!!!
        // smw.mid = tempo = 132;

        data->tempo = GetInitialTempo(data->midifile);

        data->duration = data->midifile.getFileDurationInSeconds();
        data->eventQueue = Soulcast::Audio::BuildEventQueue(data->midifile);
    }

    // UI
    {
        widget->data = this->data;
        ui->widgetLayout->addWidget(widget, 1);

        // Scrollbar
        widget->scrollbar = new QScrollBar(Qt::Vertical, ui->widgetLayout->widget());
        ui->widgetLayout->addWidget(widget->scrollbar);

        widget->Init();
    }
}

// -----------
// MusicWidget
// -----------

int fontId;
QStringList fontFamilies;
QString fontFamily;
bool madeFont = false;

MusicWidget::MusicWidget(QWidget *parent)
{
    // @HACK
    if (!madeFont)
    {
        // Load the font from a file
        fontId = QFontDatabase::addApplicationFont(":/fonts/JetBrainsMono[wght].ttf");
        if (fontId == -1)
        {
            qWarning("Failed to load font!");
        }

        // Get the family name of the loaded font
        fontFamilies = QFontDatabase::applicationFontFamilies(fontId);
        if (fontFamilies.isEmpty())
        {
            qWarning("No font families found!");
        }

        fontFamily = fontFamilies.first();

        madeFont = true;
    }
}

void MusicWidget::Init()
{
    connect(scrollbar, &QScrollBar::valueChanged, this, [this](int value)
    {
        this->panY = -value;
        update();
    });
}

void MusicWidget::clampPan()
{
    if (panX > 0)
        panX = 0;
    if (panY > 0)
        panY = 0;

    const auto min = -((height() * zoomY) - height());
    if (panY < min)
    {
        panY = min;
    }
}

void MusicWidget::updateScrollbar()
{
    scrollbar->setRange(0, (height() * zoomY) - height());
    scrollbar->setPageStep(height());
    // scrollbar->setSingleStep(20);
    scrollbar->setValue(-panY);
}

void MusicWidget::paintEvent(QPaintEvent* )
{
    QPainter p(this);

    QPen pen(qApp->palette().base(), 0);
    p.setPen(Qt::NoPen);

    p.setBrush(QColor(25, 25, 25));
    p.drawRect(0, 0, width(), height());

    const auto snapMult = 0.25f;
    const auto snapInterval = (60.0f / data->tempo) * snapMult;

    const auto zoomX = this->zoomX;
    const auto zoomY = this->zoomY;

    const auto xpos = this->panX;
    const auto ypos = this->panY;

    const auto pixelsPerSecond = 100 * zoomX;
    const auto pixelsPerBeat = pixelsPerSecond * snapInterval;

    const auto widgetWidth = width();
    const auto widgetHeight = height();

    const auto numBeats = widgetWidth / pixelsPerBeat;

    const auto pianoRowHeight = (widgetHeight / MAX_KEY_COUNT) * zoomY;
    const auto pianoRowHalfHeight = pianoRowHeight / 2;

    const auto blackKeyWidth = KEYBOARD_WIDTH - 30;

#define NOTE_TO_Y(note) ((MAX_KEY_COUNT_M1 - note) * pianoRowHeight)

    p.setRenderHint(QPainter::Antialiasing, true);

    const QColor strongBeatColor(255, 255, 255, 32);
    const QColor weakBeatColor(255, 255, 255, 10);

    const QColor whiteKeyColor(205, 209, 216);
    const QColor blackKeyColor(49, 55, 61);

    // Beat lines
    {
        const auto xOffset = Soulcast::Math::round2Nearest(-xpos, pixelsPerBeat);
        const auto xIndexOffset = static_cast<int>(round(xOffset / pixelsPerBeat));
        const auto totalTranslateX = xpos + TILES_START_X + xOffset;

        p.resetTransform();
        p.translate(totalTranslateX, 0);

        QPen pen;
        pen.setWidth(0);

        for (int i = 0; i < numBeats + 2; i++)
        {
            const auto globalIndex = i + xIndexOffset;
            pen.setColor((globalIndex % 4 == 0) ? strongBeatColor : weakBeatColor);

            p.setPen(pen);

            const auto lineX = (i * pixelsPerBeat);
            p.drawLine(lineX, 0, lineX, widgetHeight);
        }
    }

    // Draw notes
    {
        p.resetTransform();
        p.translate(xpos, ypos);
        p.translate(TILES_START_X, 0);

        const QColor trackColor0 = 0xFFFF;
        const QColor trackColor1 = QColor(220, 96, 105);
        const QColor trackColor2 = QColor(141, 187, 110);
        const QColor trackColor3 = QColor(225, 184, 111);

        const auto screenWidth = widgetWidth - TILES_START_X;

        const auto screenWidthInSeconds = (screenWidth / pixelsPerSecond);
        const auto screenMinXInSeconds = -xpos / pixelsPerSecond;
        const auto screenMaxXInSeconds = screenMinXInSeconds + screenWidthInSeconds;

        for (const auto& ev : data->eventQueue)
        {
            if (!ev.isNoteOn) continue;
            double start = ev.timeInSeconds;
            double end = 0;

            if (start > screenMaxXInSeconds)
                break;

            for (const auto& off : data->eventQueue)
            {
                if (!off.isNoteOn && off.note == ev.note && off.timeInSeconds > start)
                {
                    end = off.timeInSeconds;
                    break;
                }
            }

            if (end < screenMinXInSeconds)
                continue;

            const float x = (start * pixelsPerSecond);
            const float width = (end - start) * pixelsPerSecond;
            const float y = NOTE_TO_Y(ev.note);
            const float height = pianoRowHeight;

            auto color = trackColor1;

            if (ev.track == 3)
                color = trackColor2;
            if (ev.track == 4)
                color = trackColor3;

            p.setBrush(color);

            p.drawRoundedRect(x, y, width, height, 4, 4);
            // p.drawRect(x, y, width, height);
        }
    }

    // Row lines
    {
        QPen pen;
        pen.setColor(weakBeatColor);

        p.setPen(pen);

        p.resetTransform();
        p.translate(0, ypos);

        for (int i = 0; i < MAX_KEY_COUNT; i++)
        {
            const auto lineY = i * pianoRowHeight;
            p.drawLine(0, lineY, widgetWidth, lineY);
        }
    }

    // Draw keyboard
    if (true)
    {
        p.setPen(Qt::NoPen);

        // White keys
        {
            p.resetTransform();

            p.setBrush(whiteKeyColor);
            p.drawRect(QRect(0, 0, KEYBOARD_WIDTH, widgetHeight));
        }

        p.translate(0, ypos);

        // Draw black keys
        {
            for (int i = 0; i < MAX_KEY_COUNT; i++)
            {
                const auto y = NOTE_TO_Y(i);

                if (i % 12 == 0)
                {
                    p.save();
                    p.setPen(Qt::NoPen);
                    p.setBrush(QColor(0, 0, 0, 25));
                    p.drawRect(QRect(0, y, KEYBOARD_WIDTH, pianoRowHeight));
                    p.restore();
                }

                if (IsBlackKey(i))
                {
                    p.setBrush(blackKeyColor);
                    p.drawRect(0, y, blackKeyWidth, pianoRowHeight);
                }
            }
        }

        // Draw key labels
        {
            QPen pen(blackKeyColor);
            p.setPen(pen);

            p.setFont(QFont(qApp->font().family(), std::min(pianoRowHalfHeight, (float)(KEYBOARD_WIDTH - blackKeyWidth) / 2), QFont::Bold));

            for (int i = 0; i < MAX_KEY_COUNT; i++)
            {
                if (!IsBlackKey(i))
                {
                    const auto y = NOTE_TO_Y(i);
                    p.drawText(QRect(0, y, KEYBOARD_WIDTH - 2, pianoRowHeight), Qt::AlignRight | Qt::AlignVCenter, QString().fromStdString(GetNoteName(i)));
                }
            }
        }

        // Keyboard lines
        {
            p.setPen(QPen(blackKeyColor));

            bool lastWasBlackKey = false;
            for (int i = 0; i < MAX_KEY_COUNT; i++)
            {
                if (IsBlackKey(i))
                {
                    lastWasBlackKey = true;
                }
                else
                {
                    if (!lastWasBlackKey)
                    {
                        const auto lineY = NOTE_TO_Y(i + 1);
                        p.drawLine(0, lineY, KEYBOARD_WIDTH, lineY);
                    }

                    lastWasBlackKey = false;
                }
            }
        }

        // Keyboard separator
        {
            p.setPen(QPen(Qt::black));
            p.resetTransform();

            p.drawLine(KEYBOARD_WIDTH, 0, KEYBOARD_WIDTH, widgetHeight);
        }
    }
    else
    {
        // Keyboard separator
        {
            p.setPen(QPen(Qt::white));
            p.resetTransform();

            p.drawLine(KEYBOARD_WIDTH, 0, KEYBOARD_WIDTH, widgetHeight);
        }
    }
}

void MusicWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_panning = true;
        lastMousePos = event->pos();
        panStart = QPointF(panX, panY);
    }
}

void MusicWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_panning = false;
    }
}

void MusicWidget::mouseMoveEvent(QMouseEvent* event)
{
    if (m_panning)
    {
        auto delta = event->pos() - lastMousePos;
        panX = panStart.x() + delta.x();
        panY = panStart.y() + delta.y();

        clampPan();
        updateScrollbar();

        update();
    }
}

void MusicWidget::wheelEvent(QWheelEvent* event)
{
    float oldZoomX = zoomX;
    float oldZoomY = zoomY;

    // Zoom in/out factor
    float factor = 1.1f;
    bool zoomIn = event->angleDelta().y() > 0;

    // Determine zoom axis
    bool ctrlHeld = event->modifiers() & Qt::ControlModifier;

    // Screen position of the cursor
    QPointF cursor = event->position() - QPointF(TILES_START_X, 0);
    if (cursor.x() < 0)
    {
        cursor.setX(0);
    }

    // Apply zoom on the correct axis
    if (ctrlHeld)
    {
        zoomY *= zoomIn ? factor : (1.0f / factor);
    } else
    {
        zoomX *= zoomIn ? factor : (1.0f / factor);
    }

    zoomY = std::clamp(zoomY, 1.0f, 100000.0f);

    // Convert screen pos to world pos (before zoom change)
    QPointF worldBefore(
        (cursor.x() - panX) / oldZoomX,
        (cursor.y() - panY) / oldZoomY
        );

    // Convert world pos back to screen pos (after zoom change)
    QPointF screenAfter(
        worldBefore.x() * zoomX + panX,
        worldBefore.y() * zoomY + panY
        );

    // Adjust pan so screenAfter == cursor
    QPointF panDelta = cursor - screenAfter;
    panX += panDelta.x();
    panY += panDelta.y();

    clampPan();
    updateScrollbar();

    update();
}

void MusicWidget::resizeEvent(QResizeEvent *)
{
    clampPan();
    updateScrollbar();
}
