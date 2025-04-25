#ifndef MUSICEDITOR_HPP
#define MUSICEDITOR_HPP

#include <QWidget>
#include <QScrollBar>

#include "Engine/Core/Engine.hpp"

namespace Ui {
class MusicEditor;
}

constexpr auto TRACK_INFO_WIDTH = 93;

struct MusicData
{
    MidiFile midifile;
    double duration = 0.0;
    size_t eventIndex = 0;
    double tempo = 0;

    std::vector<Soulcast::Audio::ScheduledMidiEvent> eventQueue;
};

class PianoWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PianoWidget(QWidget* parent = nullptr);

    void Init();

    QScrollBar* scrollbar = nullptr;
    MusicData* data = nullptr;

private:
    QPointF panStart;
    QPoint lastMousePos;

    bool m_panning = false;

    float panX = 0.0f;
    float panY = 0.0f;
    float zoomX = 1;
    float zoomY = 1;

private:
    static constexpr auto KEYBOARD_WIDTH = TRACK_INFO_WIDTH;
    static constexpr auto TILES_START_X = KEYBOARD_WIDTH + 1; // The start of the piano tiles view

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
    explicit TracksWidget(QWidget* parent = nullptr);

    MusicData* data = nullptr;

protected:
    void paintEvent(QPaintEvent*) override;
};

class MusicEditor : public QWidget
{
    Q_OBJECT

public:
    explicit MusicEditor(const QString& path, QWidget *parent = nullptr);
    ~MusicEditor();

    void Init(const QString& path);

private:
    PianoWidget* pianoWidget;
    TracksWidget* tracksWidget;

    MusicData* data = nullptr;

private:
    Ui::MusicEditor *ui;
};

#endif // MUSICEDITOR_HPP
