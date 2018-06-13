#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include <QToolButton>

#ifdef WIN32
#include <QtWinExtras>
#endif

#include "style.h"

QT_FORWARD_DECLARE_CLASS(QAbstractItemView)
QT_FORWARD_DECLARE_CLASS(QLabel)
QT_FORWARD_DECLARE_CLASS(QMediaPlayer)
QT_FORWARD_DECLARE_CLASS(QModelIndex)
QT_FORWARD_DECLARE_CLASS(QPushButton)
QT_FORWARD_DECLARE_CLASS(QToolButton)
QT_FORWARD_DECLARE_CLASS(QSlider)
QT_FORWARD_DECLARE_CLASS(QVideoProbe)
QT_FORWARD_DECLARE_CLASS(QVideoWidget)
QT_FORWARD_DECLARE_CLASS(QAudioProbe)

class PlaylistModel;
class HistogramWidget;

class Player : public QWidget
{
    Q_OBJECT
private:
    void clearHistogram();
    void setTrackInfo(const QString &info);
    void setStatusInfo(const QString &info);
    void handleCursor(QMediaPlayer::MediaStatus status);
    void updateDurationInfo(qint64 currentInfo);
    void updateDurationInfo_music(qint64 currentInfo);

    QMediaPlaylist *m_playlist = nullptr;
    QMediaPlaylist *m_playlist_music = nullptr;
    QVideoWidget *m_videoWidget = nullptr;

    QLabel *m_coverLabel = nullptr;
    QSlider *m_slider = nullptr;
    QSlider *m_slider_music = nullptr;
    QLabel *m_labelDuration = nullptr;
    QLabel *m_labelDuration_music = nullptr;
    QToolButton *m_fullScreenButton = nullptr;
    QToolButton *m_colorButton = nullptr;
    QDialog *m_colorDialog = nullptr;

    HistogramWidget *m_videoHistogram = nullptr;
    HistogramWidget *m_audioHistogram = nullptr;
    QVideoProbe *m_videoProbe = nullptr;
    QAudioProbe *m_audioProbe = nullptr;

    PlaylistModel *m_playlistModel = nullptr;
    QAbstractItemView *m_playlistView = nullptr;

    PlaylistModel *m_playlistModel_music = nullptr;
    QAbstractItemView *m_playlistView_music = nullptr;

    QString m_trackInfo;
    QString m_statusInfo;
    qint64 m_duration;
    qint64 m_duration_music;

#ifdef WIN32
    void createTaskbar();
    void createThumbnailToolBar();

    QWinTaskbarProgress *m_taskbarProgress = nullptr;
    QWinTaskbarButton *m_taskbarButton = nullptr;

    QWinThumbnailToolBar *thumbnailToolBar = nullptr;
    QWinThumbnailToolButton *playToolButton = nullptr;
    QWinThumbnailToolButton *forwardToolButton = nullptr;
    QWinThumbnailToolButton *backwardToolButton = nullptr;
#endif

private slots:
    void open();
    void open_music();
    void del();
    void del_music();
    void durationChanged(qint64 duration);
    void positionChanged(qint64 progress);
    void durationChanged_music(qint64 duration);
    void positionChanged_music(qint64 progress);
    void metaDataChanged();
    void metaDataChanged_music();

    void previousClicked();

    void seek(int seconds);
    void seek_music(int seconds);
    void jump(const QModelIndex &index);
    void jump_music(const QModelIndex &index);
    void playlistPositionChanged(int);
    void playlistPositionChanged_music(int);

    void statusChanged(QMediaPlayer::MediaStatus status);
    void stateChanged(QMediaPlayer::State state);
    void bufferingProgress(int progress);
    void videoAvailableChanged(bool available);

    void displayErrorMessage();

    void showColorDialog();

#ifdef WIN32
    void updateTaskbar();
    void updateThumbnailToolBar();
#endif

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

    QMediaPlayer *m_player = nullptr;
    QMediaPlayer *m_player_music = nullptr;

    bool isPlayerAvailable() const;

    void addToPlaylist(const QList<QUrl> &urls);
    void addToPlaylist_music(const QList<QUrl> &urls);

public slots:
    void togglePlayback();
    void seekForward();
    void seekBackward();

signals:
    void fullScreenChanged(bool fullScreen);


};
#endif // PLAYER_H
