#ifndef MUSICEDITOR_HPP
#define MUSICEDITOR_HPP

#include <QWidget>
#include <QScrollBar>
#include <QTimer>

#include "Engine/Core/Engine.hpp"

namespace Ui {
class MusicEditor;
}

constexpr auto TRACK_INFO_WIDTH = 93;
const QColor COLOR_SEPARATOR = QColor(22, 22, 22);

struct MidiNote
{
    double timeInSeconds;
    double endTimeInSeconds;

    int note;
};

struct AudioState
{
    double time = 0.0;
    bool active = false;
    bool silenced = false;

    struct Voice
    {
        int channel = 0;
        bool active = false;
        bool silenced = false;
        int currentNote = -1;
        int currentIndex = 0;
        double currentFrequency = 0.0;
    };

    std::vector<Voice> tracks;

    void resize(int numTracks)
    {
        tracks.resize(numTracks);
    }
};

struct ScheduledMidiEvent
{
    double timeInSeconds;
    int track;
    int note;
    bool isNoteOn;
};

struct MusicData
{
    MidiFile midifile;
    double duration = 0.0;
    size_t eventIndex = 0;
    double tempo = 0;

    double songPosition;

    AudioState state;

    std::vector<ScheduledMidiEvent> eventQueue;
    std::vector<std::vector<MidiNote>> tracks;
};

class MusicEditor;
class PianoWidget;
class TracksWidget;

class MusicEditor : public QWidget
{
    Q_OBJECT

public:
    explicit MusicEditor(const QString& path, QWidget* parent = nullptr);
    ~MusicEditor();

    void Init(const QString& path);
    void Start();
    void Stop();

    QTimer playbackTimer;
    bool m_playing = false;
    double startTime = 0.0;

    void SwitchTrack(int track);
    int currentTrack = 0;

private slots:
    void ProcessAudio();

private:
    PianoWidget* pianoWidget;
    TracksWidget* tracksWidget;

    MusicData* data = nullptr;

private:
    Ui::MusicEditor* ui;
};

class PianoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PianoWidget(MusicEditor* editor, QWidget* parent = nullptr);

    void Init();

    MusicEditor* editor = nullptr;
    QScrollBar* scrollbar = nullptr;
    MusicData* data = nullptr;

private:
    QPointF panStart;
    QPoint lastMousePos;

    bool m_panning = false;

    float panX = 0.0f;
    float panY = 0.0f;
    float zoomX = 1;
    float zoomY = 4;

private:
    static constexpr auto KEYBOARD_WIDTH = TRACK_INFO_WIDTH;
    static constexpr auto TIMELINE_HEIGHT = 12;

    static constexpr auto TILES_START_X = KEYBOARD_WIDTH + 1; // The start of the piano tiles view
    static constexpr auto TILES_START_Y = TIMELINE_HEIGHT + 1;

    static constexpr auto MAX_KEY_COUNT = 132.0f;
    static constexpr auto MAX_KEY_COUNT_M1 = MAX_KEY_COUNT - 1;

private:
    void clampPan();
    void updateScrollbar();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void wheelEvent(QWheelEvent*) override;
    void resizeEvent(QResizeEvent*) override;
};

class TracksWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TracksWidget(MusicEditor* editor, QWidget* parent = nullptr);

    MusicEditor* editor = nullptr;
    MusicData* data = nullptr;

    short m_highlight = -1;

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent*) override;
    void mouseMoveEvent(QMouseEvent*) override;
};

#endif // MUSICEDITOR_HPP
