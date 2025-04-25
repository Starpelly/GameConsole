#include "MusicEditor.hpp"
#include "ui_musiceditor.h"

#include <QPainter>
#include <QEvent>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

#include <QFontDatabase>
#include <QDateTime>

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
    , pianoWidget(new PianoWidget(this))
    , tracksWidget(new TracksWidget(this))
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
    // UI
    {
        // Tracks
        tracksWidget->data = this->data;
        tracksWidget->setMaximumHeight(200);
        ui->tracksContainer->addWidget(tracksWidget, 1);

        // Piano Roll
        pianoWidget->data = this->data;
        ui->pianoRollContainer->addWidget(pianoWidget, 1);

        // Scrollbar
        pianoWidget->scrollbar = new QScrollBar(Qt::Vertical, ui->pianoRollContainer->widget());
        ui->pianoRollContainer->addWidget(pianoWidget->scrollbar);

        pianoWidget->Init();
    }

    // Actions
    {
        connect(ui->playButton, &QPushButton::pressed, this, [this]()
        {
            if (!m_playing)
                Start();
            else
                Stop();

            ui->playButton->setText(m_playing ? "Stop" : "Play");
        });
    }

    // Music data
    {
        data->midifile.read(path.toStdString());
        data->midifile.doTimeAnalysis();

        // @Note: sometimes, getting the initial tempo will not work!
        // Pls fix!!!
        // smw.mid = tempo = 132;

        data->tempo = GetInitialTempo(data->midifile);

        data->duration = data->midifile.getFileDurationInSeconds();
        // data->eventQueue = Soulcast::Audio::BuildEventQueue(data->midifile);

        data->eventQueue = Soulcast::Audio::BuildEventQueue(data->midifile);

        data->state.resize(data->midifile.getTrackCount());

        auto& midi = data->midifile;

        const auto numTracks = midi.getTrackCount();

        data->tracks.resize(numTracks);
        for (int t = 0; t < numTracks; ++t)
        {
            auto& track = midi[t];
            for (int i = 0; i < track.size(); i++)
            {
                auto& ev = track[i];

                if (ev.isNoteOn())
                {
                    auto endSeconds = 0.0f;

                    for (int ii = i; ii < track.size(); ii++)
                    {
                        auto& off = track[ii];
                        if (!off.isNoteOn() && off.getKeyNumber() == ev.getKeyNumber() && off.seconds > ev.seconds)
                        {
                            endSeconds = off.seconds;
                            break;
                        }
                    }

                    data->tracks[t].push_back({ ev.seconds, endSeconds, ev.getKeyNumber() });
                }
            }
        }
    }

    Soulcast::AudioDevice::Init(&data->state);
}

void MusicEditor::Start()
{
    data->eventIndex = 0;
    data->state.active = true;

    startTime = QDateTime::currentMSecsSinceEpoch() / 1000.0;

    connect(&playbackTimer, &QTimer::timeout, this, &MusicEditor::ProcessAudio);

    playbackTimer.start(1);

    m_playing = true;
}

void MusicEditor::Stop()
{
    data->songPosition = 0.0;
    data->eventIndex = 0;
    data->state.active = false;

    playbackTimer.stop();
    m_playing = false;
}

void MusicEditor::ProcessAudio()
{
    double now = QDateTime::currentMSecsSinceEpoch() / 1000.0;
    double elapsed = now - startTime;
    data->songPosition = elapsed;

    Soulcast::AudioDevice::ProcessMIDI(data->songPosition, data->eventQueue, data->state, data->eventIndex);
    update();
}

// -----------
// MusicWidget
// -----------

int fontId;
QStringList fontFamilies;
QString fontFamily;
bool madeFont = false;

PianoWidget::PianoWidget(QWidget *parent)
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

void PianoWidget::Init()
{
    connect(scrollbar, &QScrollBar::valueChanged, this, [this](int value)
    {
        this->panY = -value;
        update();
    });
}

void PianoWidget::clampPan()
{
    if (panX > 0)
        panX = 0;
    if (panY > 0)
        panY = 0;

    const auto viewHeight = height() - TILES_START_Y;

    const auto min = -((viewHeight * zoomY) - viewHeight);
    if (panY < min)
    {
        panY = min;
    }
}

void PianoWidget::updateScrollbar()
{
    const auto viewHeight = height() - TILES_START_Y;

    scrollbar->setRange(0, (viewHeight * zoomY) - viewHeight);
    scrollbar->setPageStep(height());
    // scrollbar->setSingleStep(20);
    scrollbar->setValue(-panY);
}

void PianoWidget::paintEvent(QPaintEvent* )
{
    QPainter p(this);

    p.setPen(Qt::NoPen);

    p.setBrush(qApp->palette().dark());
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

    const auto screenWidth = widgetWidth - TILES_START_X;
    const auto screenHeight = widgetHeight - TILES_START_Y;

    const auto numBeats = widgetWidth / pixelsPerBeat;

    const auto pianoRowHeight = (screenHeight / MAX_KEY_COUNT) * zoomY;
    const auto pianoRowHalfHeight = pianoRowHeight / 2;

    const auto blackKeyWidth = KEYBOARD_WIDTH - 30;

#define NOTE_TO_Y(note) ((MAX_KEY_COUNT_M1 - note) * pianoRowHeight)

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
        p.setRenderHint(QPainter::Antialiasing, true);

        p.resetTransform();
        p.translate(xpos, ypos);
        p.translate(TILES_START_X, TILES_START_Y);

        const QColor trackColor0 = 0xFFFF;
        const QColor trackColor1 = QColor(220, 96, 105);
        const QColor trackColor2 = QColor(141, 187, 110);
        const QColor trackColor3 = QColor(225, 184, 111);

        const auto screenWidthInSeconds = (screenWidth / pixelsPerSecond);
        const auto screenMinXInSeconds = -xpos / pixelsPerSecond;
        const auto screenMaxXInSeconds = screenMinXInSeconds + screenWidthInSeconds;

        auto drawTrack = [&](int t)
        {
            const int startIndex = 0;
            const auto& track = data->tracks[t];

            for (int i = startIndex; i < track.size(); i++)
            {
                const auto& ev = track[i];

                const double start = ev.timeInSeconds;
                const double end = ev.endTimeInSeconds;

                if (start > screenMaxXInSeconds)
                    break;
                if (end < screenMinXInSeconds)
                    continue;

                const float x = (start * pixelsPerSecond);
                const float width = (end - start) * pixelsPerSecond;
                const float y = NOTE_TO_Y(ev.note);
                const float height = pianoRowHeight;

                auto color = trackColor1;

                if (t == 3)
                    color = trackColor2;
                if (t == 4)
                    color = trackColor3;

                p.setBrush(color);

                p.drawRoundedRect(x, y, width, height, 4, 4);
                // p.drawRect(x, y, width, height);
            }
        };


        // for (const auto& ev : data->eventQueue)
        for (int t = 0; t < data->tracks.size(); t++)
        {
            // drawTrack(t);
        }
        drawTrack(4);

        p.setRenderHint(QPainter::Antialiasing, false);
    }

    // Row lines
    {
        QPen pen;
        pen.setColor(weakBeatColor);

        p.setPen(pen);

        p.resetTransform();
        p.translate(0, ypos + TILES_START_Y);

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

        p.translate(0, ypos + TILES_START_Y);

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

            p.setFont(QFont(qApp->font().family(), std::min(pianoRowHalfHeight, (float)(KEYBOARD_WIDTH - blackKeyWidth) / 2), QFont::Normal));

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
            p.setPen(QPen(qApp->palette().window().color()));
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

            p.drawLine(KEYBOARD_WIDTH, 0, KEYBOARD_WIDTH, TIMELINE_HEIGHT);
        }
    }

    // Draw Timeline
    {
        p.resetTransform();

        p.setPen(Qt::NoPen);
        p.drawRect(0, 0, widgetWidth, TILES_START_Y);

        p.setBrush(QColor(0, 0, 0));
        p.drawRect(TILES_START_X, 0, widgetWidth, TILES_START_Y);

        // Timed shit
        p.save();
        {
            p.setClipping(true);
            p.setClipRect(TILES_START_X, 0, widgetWidth, widgetHeight);

            p.translate(xpos, 0);
            p.translate(TILES_START_X, 0);

            // Playback head
            p.save();
            {
                const auto playbackX = data->songPosition * pixelsPerSecond;

                QPen pen(qApp->palette().highlightedText().color());
                pen.setWidth(2);
                p.setPen(pen);
                p.drawLine(playbackX, 0, playbackX, widgetHeight);
            }
            p.restore();
        }
        p.restore();
    }
}

void PianoWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_panning = true;
        lastMousePos = event->pos();
        panStart = QPointF(panX, panY);
    }
}

void PianoWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::MiddleButton)
    {
        m_panning = false;
    }
}

void PianoWidget::mouseMoveEvent(QMouseEvent* event)
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

void PianoWidget::wheelEvent(QWheelEvent* event)
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

void PianoWidget::resizeEvent(QResizeEvent *)
{
    clampPan();
    updateScrollbar();
}

// ------------
// TracksWidget
// ------------

TracksWidget::TracksWidget(QWidget *parent)
{

}

void TracksWidget::paintEvent(QPaintEvent *)
{
    const auto CHANNEL_COUNT = Soulcast::Audio::CHANNEL_COUNT;

    const auto zoomX = 1;
    const auto zoomY = 1;

    const auto channelRowHeight = (height() / CHANNEL_COUNT) * zoomY;

    QPainter p(this);
    p.setPen(Qt::NoPen);

    p.setBrush(qApp->palette().base());
    p.drawRect(0, 0, width(), height());

    // p.setPen(QPen(qApp->palette().window().color()));
    p.setPen(Qt::black);
    p.drawLine(TRACK_INFO_WIDTH, 0, TRACK_INFO_WIDTH, height());

    // Draw channels
    {
        p.setPen(Qt::black);
        for (int i = 0; i < CHANNEL_COUNT; i++)
        {
            const auto yPos = i * channelRowHeight;
            p.drawLine(0, yPos, width(), yPos);
        }
    }
}
