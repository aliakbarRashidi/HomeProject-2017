#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QStandardItemModel>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QMouseEvent>
#include <QFileDialog>
#include <QDir>
#include <QGraphicsDropShadowEffect>
#include <QMediaMetaData>

#ifdef WIN32
#include <QtWinExtras>
#endif

#include "volumebutton.h"
#include "histogramwidget.h"
#include "player.h"
#include "style.h"

QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QWinTaskbarButton)
QT_FORWARD_DECLARE_CLASS(QWinTaskbarProgress)
QT_FORWARD_DECLARE_CLASS(QAudioProbe)

class HistogramWidget;
class VolumeButton;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT
    // Свойство с точкой предыдущей позиции мыши
    // Относительно данной точки идёт пересчёт позиции окна
    // Или размеров окна. При этом свойство устанавливается при нажатии мыши
    // по окну и в ряде иных случаев
    Q_PROPERTY(QPoint previousPosition READ previousPosition WRITE setPreviousPosition NOTIFY previousPositionChanged)

    // тип клика мыши, при перемещении курсора по этому типу будем определять
    // что именно нужно сделать, перенести окно, или изменить его размер с одной из сторон
    enum MouseType {
        None = 0,
        Top,
        Bottom,
        Left,
        Right,
        Move
    };

private:
    Ui::Widget *ui = nullptr;
    Player *player = nullptr;

    HistogramWidget *m_audioHistogram = nullptr;
    QAudioProbe *m_audioProbe = nullptr;

    QStandardItemModel *m_playListModel = nullptr;
    QMediaPlayer *m_player = nullptr;
    QMediaPlaylist *m_playlist = nullptr;

    VolumeButton *m_volumeButton = nullptr;

#ifdef WIN32
    QWinTaskbarProgress *m_taskbarProgress = nullptr;
    QWinTaskbarButton *m_taskbarButton = nullptr;

    QWinThumbnailToolBar *thumbnailToolBar = nullptr;
    QWinThumbnailToolButton *playToolButton = nullptr;
    QWinThumbnailToolButton *forwardToolButton = nullptr;
    QWinThumbnailToolButton *backwardToolButton = nullptr;
#endif

    // Переменная, от которой будем отталкиваться при работе с перемещением и изменением размера окна
    MouseType m_leftMouseButtonPressed;
    QPoint m_previousPosition;

    MouseType checkResizableField(QMouseEvent*);

#ifdef WIN32
    void createTaskbar();
    void createThumbnailToolBar();
#endif
    void clearHistogram();

private slots:
    void on_btn_add_clicked();
    void on_btn_del_clicked();
    void on_btn_random_clicked();

    void updatePosition(qint64);
    void updateDuration(qint64);
    void setPosition(int);

#ifdef WIN32
    void updateTaskbar();
    void updateThumbnailToolBar();
#endif

    void on_btn_music_clicked();
    void on_btn_video_clicked();

    void on_btn_close_clicked();

protected:
    void mousePressEvent(QMouseEvent*);
    void mouseReleaseEvent(QMouseEvent*);
    void mouseMoveEvent(QMouseEvent*);

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    QPoint previousPosition() const;
    void addToPlaylist(const QList<QUrl> &urls);

public slots:
    void setPreviousPosition(QPoint);

    void togglePlayback();
    void seekForward();
    void seekBackward();

signals:
    void previousPositionChanged(QPoint);
};

#endif // WIDGET_H
