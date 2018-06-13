#include <QMediaService>
#include <QMediaPlaylist>
#include <QVideoProbe>
#include <QAudioProbe>
#include <QMediaMetaData>
#include <QtWidgets>

#include "player.h"

#include "playercontrols.h"
#include "playlistmodel.h"
#include "histogramwidget.h"
#include "videowidget.h"


Player::Player(QWidget *parent)
    : QWidget(parent)
{
    this->setStyleSheet(Style::getWindowStyleSheet());
    this->setWindowTitle("Video player");

    this->setFixedSize(700, 250);


    setWindowFlags(Qt::Window
        | Qt::WindowTitleHint
        | Qt::WindowSystemMenuHint
        | Qt::WindowMinimizeButtonHint
        | Qt::WindowFullscreenButtonHint
        | Qt::CustomizeWindowHint);

    m_player = new QMediaPlayer(this);
    m_player_music = new QMediaPlayer(this);
    m_player->setAudioRole(QAudio::VideoRole);
    m_player_music->setAudioRole(QAudio::MusicRole);

    m_playlist = new QMediaPlaylist();
    m_playlist_music = new QMediaPlaylist();
    m_player->setPlaylist(m_playlist);
    m_player_music->setPlaylist(m_playlist_music);

    m_videoWidget = new VideoWidget(this);
    m_player->setVideoOutput(m_videoWidget);

    m_playlistModel = new PlaylistModel(this);
    m_playlistModel->setPlaylist(m_playlist);

    m_playlistModel_music = new PlaylistModel(this);
    m_playlistModel_music->setPlaylist(m_playlist_music);

    m_playlistView = new QListView(this);
    m_playlistView_music = new QListView(this);

    m_playlistView->setModel(m_playlistModel);
    m_playlistView_music->setModel(m_playlistModel_music);

    m_playlistView->setCurrentIndex(m_playlistModel->index(m_playlist->currentIndex(), 0));
    m_playlistView_music->setCurrentIndex(m_playlistModel_music->index(m_playlist_music->currentIndex(), 0));

    m_playlistView->verticalScrollBar()->setStyleSheet(Style::getVerticalScrollBarStyleSheet());
    m_playlistView->horizontalScrollBar()->setStyleSheet(Style::getHorizontalScrollBarStyleSheet());

    m_playlistView_music->verticalScrollBar()->setStyleSheet(Style::getVerticalScrollBarStyleSheet());
    m_playlistView_music->horizontalScrollBar()->setStyleSheet(Style::getHorizontalScrollBarStyleSheet());


    m_slider = new QSlider(Qt::Horizontal, this);
    m_slider->setStyleSheet(Style::getSliderStyleSheet());
    m_slider->setRange(0, m_player->duration() / 1000);

    m_slider_music = new QSlider(Qt::Horizontal, this);
    m_slider_music->setStyleSheet(Style::getSliderStyleSheet());
    m_slider_music->setRange(0, m_player_music->duration() / 1000);

    m_labelDuration = new QLabel(this);
    m_labelDuration_music = new QLabel(this);

    m_videoProbe = new QVideoProbe(this);
    m_audioProbe = new QAudioProbe(this);

    m_videoHistogram = new HistogramWidget(this);
    m_audioHistogram = new HistogramWidget(this);
    QHBoxLayout *histogramLayout = new QHBoxLayout;
    histogramLayout->addWidget(m_videoHistogram, 1);
    histogramLayout->addWidget(m_audioHistogram, 2);

    QToolButton *openButton = new QToolButton(this);
    QToolButton *openButton_music = new QToolButton(this);
    openButton->setStyleSheet(Style::getAddStyleSheet());
    openButton_music->setStyleSheet(Style::getAddStyleSheet());

    QToolButton *delButton = new QToolButton(this);
    QToolButton *delButton_music = new QToolButton(this);
    delButton->setStyleSheet(Style::getRemoveStyleSheet());
    delButton_music->setStyleSheet(Style::getRemoveStyleSheet());

    PlayerControls *controls = new PlayerControls(this);
    PlayerControls *controls_music = new PlayerControls(this);

    controls->setState(m_player->state());
    controls->setVolume(m_player->volume());
    controls->setMuted(controls->isMuted());

    controls_music->setState(m_player_music->state());
    controls_music->setVolume(m_player_music->volume());
    controls_music->setMuted(controls_music->isMuted());

    m_fullScreenButton = new QToolButton(this);
    m_fullScreenButton->setStyleSheet(Style::getFullStyleSheet());
    m_fullScreenButton->setCheckable(true);

    m_colorButton = new QToolButton(this);
    m_colorButton->setEnabled(false);
    m_colorButton->setStyleSheet(Style::getSettingsStyleSheet());
    connect(m_colorButton, &QPushButton::clicked, this, &Player::showColorDialog);

    connect(m_slider, &QSlider::sliderMoved, this, &Player::seek);
    connect(m_slider_music, &QSlider::sliderMoved, this, &Player::seek_music);

    connect(openButton, &QPushButton::clicked, this, &Player::open);
    connect(openButton_music, &QPushButton::clicked, this, &Player::open_music);

    connect(delButton, &QPushButton::clicked, this, &Player::del);
    connect(delButton_music, &QPushButton::clicked, this, &Player::del_music);
    connect(delButton, &QToolButton::clicked, m_player, &QMediaPlayer::stop);
    connect(delButton_music, &QToolButton::clicked, m_player_music, &QMediaPlayer::stop);

    connect(m_videoProbe, &QVideoProbe::videoFrameProbed, m_videoHistogram, &HistogramWidget::processFrame);
    m_videoProbe->setSource(m_player);

    connect(m_audioProbe, &QAudioProbe::audioBufferProbed, m_audioHistogram, &HistogramWidget::processBuffer);
    m_audioProbe->setSource(m_player_music);

    connect(m_playlistView, &QAbstractItemView::activated, this, &Player::jump);
    connect(m_playlistView_music, &QAbstractItemView::activated, this, &Player::jump_music);

    connect(controls, &PlayerControls::play, m_player, &QMediaPlayer::play);
    connect(controls, &PlayerControls::play, m_player_music, &QMediaPlayer::play);
    connect(controls, &PlayerControls::pause, m_player, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::pause, m_player_music, &QMediaPlayer::pause);
    connect(controls, &PlayerControls::stop, m_player, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::stop, m_player_music, &QMediaPlayer::stop);
    connect(controls, &PlayerControls::next, m_playlist, &QMediaPlaylist::next);
    connect(controls, &PlayerControls::previous, this, &Player::previousClicked);
    connect(controls, &PlayerControls::changeVolume, m_player, &QMediaPlayer::setVolume);
    connect(controls, &PlayerControls::changeMuting, m_player, &QMediaPlayer::setMuted);
    connect(controls, &PlayerControls::changeRate, m_player, &QMediaPlayer::setPlaybackRate);
    connect(controls, &PlayerControls::stop, m_videoWidget, QOverload<>::of(&QVideoWidget::update));

    connect(controls_music, &PlayerControls::play, m_player_music, &QMediaPlayer::play);
    connect(controls_music, &PlayerControls::pause, m_player_music, &QMediaPlayer::pause);
    connect(controls_music, &PlayerControls::stop, m_player_music, &QMediaPlayer::stop);
    connect(controls_music, &PlayerControls::next, m_playlist_music, &QMediaPlaylist::next);
    connect(controls_music, &PlayerControls::previous, m_playlist_music, &QMediaPlaylist::previous);
    connect(controls_music, &PlayerControls::changeVolume, m_player_music, &QMediaPlayer::setVolume);
    connect(controls_music, &PlayerControls::changeMuting, m_player_music, &QMediaPlayer::setMuted);
    connect(controls_music, &PlayerControls::changeRate, m_player_music, &QMediaPlayer::setPlaybackRate);

    connect(m_player, &QMediaPlayer::stateChanged, controls, &PlayerControls::setState);
    connect(m_player, &QMediaPlayer::volumeChanged, controls, &PlayerControls::setVolume);
    connect(m_player, &QMediaPlayer::mutedChanged, controls, &PlayerControls::setMuted);

    connect(m_player_music, &QMediaPlayer::stateChanged, controls_music, &PlayerControls::setState);
    connect(m_player_music, &QMediaPlayer::volumeChanged, controls_music, &PlayerControls::setVolume);
    connect(m_player_music, &QMediaPlayer::mutedChanged, controls_music, &PlayerControls::setMuted);

    connect(m_player, &QMediaPlayer::durationChanged, this, &Player::durationChanged);
    connect(m_player, &QMediaPlayer::positionChanged, this, &Player::positionChanged);
    connect(m_player, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &Player::metaDataChanged);
    connect(m_playlist, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged);
    connect(m_player, &QMediaPlayer::mediaStatusChanged, this, &Player::statusChanged);
    connect(m_player, &QMediaPlayer::bufferStatusChanged, this, &Player::bufferingProgress);
    connect(m_player, &QMediaPlayer::videoAvailableChanged, this, &Player::videoAvailableChanged);
    connect(m_player, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);
    connect(m_player, &QMediaPlayer::stateChanged, this, &Player::stateChanged);

    connect(m_player_music, &QMediaPlayer::durationChanged, this, &Player::durationChanged_music);
    connect(m_player_music, &QMediaPlayer::positionChanged, this, &Player::positionChanged_music);
    connect(m_player_music, QOverload<>::of(&QMediaPlayer::metaDataChanged), this, &Player::metaDataChanged_music);
    connect(m_playlist_music, &QMediaPlaylist::currentIndexChanged, this, &Player::playlistPositionChanged_music);
    connect(m_player_music, &QMediaPlayer::mediaStatusChanged, this, &Player::statusChanged);
    connect(m_player_music, &QMediaPlayer::bufferStatusChanged, this, &Player::bufferingProgress);
    connect(m_player_music, QOverload<QMediaPlayer::Error>::of(&QMediaPlayer::error), this, &Player::displayErrorMessage);
    connect(m_player_music, &QMediaPlayer::stateChanged, this, &Player::stateChanged);

    QBoxLayout *displayLayout = new QHBoxLayout;
    displayLayout->addWidget(m_videoWidget, 2);
    displayLayout->addWidget(m_playlistView);
    displayLayout->addWidget(m_playlistView_music);

    QBoxLayout *controlLayout = new QHBoxLayout;
    controlLayout->setMargin(0);
    controlLayout->addWidget(openButton);
    controlLayout->addWidget(delButton);
    controlLayout->addWidget(controls);
    controlLayout->addWidget(m_fullScreenButton);
    controlLayout->addWidget(m_colorButton);
    controlLayout->addStretch(1);

    QBoxLayout *controlLayout_music = new QHBoxLayout;
    controlLayout_music->setMargin(0);
    controlLayout_music->addWidget(openButton_music);
    controlLayout_music->addWidget(delButton_music);
    controlLayout_music->addWidget(controls_music);
    controlLayout_music->addStretch(1);

    QBoxLayout *layout = new QVBoxLayout;
    layout->addLayout(displayLayout);
    QHBoxLayout *hLayout = new QHBoxLayout;
    hLayout->addWidget(m_slider);
    hLayout->addWidget(m_labelDuration);
    QHBoxLayout *hLayout_music = new QHBoxLayout;
    hLayout_music->addWidget(m_slider_music);
    hLayout_music->addWidget(m_labelDuration_music);
    layout->addLayout(hLayout);
    layout->addLayout(hLayout_music);
    layout->addLayout(controlLayout);
    layout->addLayout(controlLayout_music);
    layout->addLayout(histogramLayout);

    m_slider->setVisible(false);
    m_labelDuration->setVisible(false);
    m_slider_music->setVisible(false);
    m_labelDuration_music->setVisible(false);

    setLayout(layout);

#ifdef WIN32
    createTaskbar();
    createThumbnailToolBar();
#endif

    metaDataChanged();
}

Player::~Player()
{
}

bool Player::isPlayerAvailable() const
{
    return m_player->isAvailable();
}

void Player::open()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList supportedMimeTypes;
    supportedMimeTypes << "video/avi"
                << "video/mp4";

    if (!supportedMimeTypes.isEmpty()) {
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }

    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MoviesLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist(fileDialog.selectedUrls());

    m_slider->setVisible(true);
    m_labelDuration->setVisible(true);
}

void Player::open_music()
{
    QFileDialog fileDialog(this);
    fileDialog.setAcceptMode(QFileDialog::AcceptOpen);

    QStringList supportedMimeTypes;
    supportedMimeTypes << "audio/mp3"
                << "audio/wav";

    if (!supportedMimeTypes.isEmpty()) {
        fileDialog.setMimeTypeFilters(supportedMimeTypes);
    }
    fileDialog.setDirectory(QStandardPaths::standardLocations(QStandardPaths::MusicLocation).value(0, QDir::homePath()));
    if (fileDialog.exec() == QDialog::Accepted)
        addToPlaylist_music(fileDialog.selectedUrls());

    m_slider_music->setVisible(true);
    m_labelDuration_music->setVisible(true);
}

void Player::del_music()
{
    m_playlist_music->removeMedia(m_playlistView_music->currentIndex().row());
    m_playlistModel_music->removeRow(m_playlistView_music->currentIndex().row());
    clearHistogram();
}

void Player::del()
{
    m_playlist->removeMedia(m_playlistView->currentIndex().row());
    m_playlistModel->removeRow(m_playlistView->currentIndex().row());
    clearHistogram();
}

static bool isPlaylist(const QUrl &url)
{
    if (!url.isLocalFile())
        return false;
    const QFileInfo fileInfo(url.toLocalFile());
    return fileInfo.exists() && !fileInfo.suffix().compare(QLatin1String("m3u"), Qt::CaseInsensitive);
}

void Player::addToPlaylist(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            m_playlist->load(url);
        else
            m_playlist->addMedia(url);
    }
}

void Player::addToPlaylist_music(const QList<QUrl> &urls)
{
    for (auto &url: urls) {
        if (isPlaylist(url))
            m_playlist_music->load(url);
        else
            m_playlist_music->addMedia(url);
    }
}

void Player::durationChanged(qint64 duration)
{
    m_duration = duration / 1000;
    m_slider->setMaximum(m_duration);
}

void Player::positionChanged(qint64 progress)
{
    if (!m_slider->isSliderDown())
        m_slider->setValue(progress / 1000);

    updateDurationInfo(progress / 1000);
}

void Player::durationChanged_music(qint64 duration)
{
    m_duration_music = duration / 1000;
    m_slider_music->setMaximum(m_duration_music);
}

void Player::positionChanged_music(qint64 progress)
{
    if (!m_slider_music->isSliderDown())
        m_slider_music->setValue(progress / 1000);

    updateDurationInfo_music(progress / 1000);
}

void Player::metaDataChanged()
{
    if (m_player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(m_player->metaData(QMediaMetaData::AlbumArtist).toString())
                .arg(m_player->metaData(QMediaMetaData::Title).toString()));

        if (m_coverLabel) {
            QUrl url = m_player->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            m_coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }

    }
}

void Player::metaDataChanged_music()
{
    if (m_player->isMetaDataAvailable()) {
        setTrackInfo(QString("%1 - %2")
                .arg(m_player_music->metaData(QMediaMetaData::AlbumArtist).toString())
                .arg(m_player_music->metaData(QMediaMetaData::Title).toString()));

        if (m_coverLabel) {
            QUrl url = m_player_music->metaData(QMediaMetaData::CoverArtUrlLarge).value<QUrl>();

            m_coverLabel->setPixmap(!url.isEmpty()
                    ? QPixmap(url.toString())
                    : QPixmap());
        }

    }
}

void Player::previousClicked()
{
    ///Переход к предыдущему треку, если менее 5 секунд, иначе в начало
    if (m_player->position() <= 5000)
        m_playlist->previous();
    else
        m_player->setPosition(0);
}

void Player::jump(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist->setCurrentIndex(index.row());
        m_player->play();
    }
}

void Player::jump_music(const QModelIndex &index)
{
    if (index.isValid()) {
        m_playlist_music->setCurrentIndex(index.row());
        m_player_music->play();
    }
}

void Player::playlistPositionChanged(int currentItem)
{
    clearHistogram();
    m_playlistView->setCurrentIndex(m_playlistModel->index(currentItem, 0));
}

void Player::playlistPositionChanged_music(int currentItem)
{
    clearHistogram();
    m_playlistView_music->setCurrentIndex(m_playlistModel_music->index(currentItem, 0));
}

void Player::seek(int seconds)
{
    m_player->setPosition(seconds * 1000);
}

void Player::seek_music(int seconds)
{
    m_player_music->setPosition(seconds * 1000);
}

void Player::statusChanged(QMediaPlayer::MediaStatus status)
{
    handleCursor(status);

    switch (status) {
    case QMediaPlayer::UnknownMediaStatus:
    case QMediaPlayer::NoMedia:
    case QMediaPlayer::LoadedMedia:
    case QMediaPlayer::BufferingMedia:
    case QMediaPlayer::BufferedMedia:
        setStatusInfo(QString());
        break;
    case QMediaPlayer::LoadingMedia:
        setStatusInfo(tr("Loading..."));
        break;
    case QMediaPlayer::StalledMedia:
        setStatusInfo(tr("Media Stalled"));
        break;
    case QMediaPlayer::EndOfMedia:
        QApplication::alert(this);
        break;
    case QMediaPlayer::InvalidMedia:
        displayErrorMessage();
        break;
    }
}

void Player::stateChanged(QMediaPlayer::State state)
{
    if (state == QMediaPlayer::StoppedState)
        clearHistogram();
}

void Player::handleCursor(QMediaPlayer::MediaStatus status)
{
#ifndef QT_NO_CURSOR
    if (status == QMediaPlayer::LoadingMedia ||
        status == QMediaPlayer::BufferingMedia ||
        status == QMediaPlayer::StalledMedia)
        setCursor(QCursor(Qt::BusyCursor));
    else
        unsetCursor();
#endif
}

void Player::bufferingProgress(int progress)
{
    setStatusInfo(tr("Buffering %4%").arg(progress));
}

void Player::videoAvailableChanged(bool available)
{
    if (!available) {
        disconnect(m_fullScreenButton, &QPushButton::clicked, m_videoWidget, &QVideoWidget::setFullScreen);
        disconnect(m_videoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);
        m_videoWidget->setFullScreen(false);
    } else {
        connect(m_fullScreenButton, &QPushButton::clicked, m_videoWidget, &QVideoWidget::setFullScreen);
        connect(m_videoWidget, &QVideoWidget::fullScreenChanged, m_fullScreenButton, &QPushButton::setChecked);

        if (m_fullScreenButton->isChecked())
            m_videoWidget->setFullScreen(true);
    }
    m_colorButton->setEnabled(available);
}

void Player::setTrackInfo(const QString &info)
{
   m_trackInfo = info;
    if (!m_statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        setWindowTitle(m_trackInfo);
}

void Player::setStatusInfo(const QString &info)
{
   m_statusInfo = info;
    if (!m_statusInfo.isEmpty())
        setWindowTitle(QString("%1 | %2").arg(m_trackInfo).arg(m_statusInfo));
    else
        setWindowTitle(m_trackInfo);
}

void Player::displayErrorMessage()
{
    setStatusInfo(m_player->errorString());
}

void Player::updateDurationInfo_music(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration_music) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration_music / 3600) % 60, (m_duration_music / 60) % 60,
            m_duration_music % 60, (m_duration_music * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration_music > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration_music->setText(tStr);
}

void Player::updateDurationInfo(qint64 currentInfo)
{
    QString tStr;
    if (currentInfo || m_duration) {
        QTime currentTime((currentInfo / 3600) % 60, (currentInfo / 60) % 60,
            currentInfo % 60, (currentInfo * 1000) % 1000);
        QTime totalTime((m_duration / 3600) % 60, (m_duration / 60) % 60,
            m_duration % 60, (m_duration * 1000) % 1000);
        QString format = "mm:ss";
        if (m_duration > 3600)
            format = "hh:mm:ss";
        tStr = currentTime.toString(format) + " / " + totalTime.toString(format);
    }
    m_labelDuration->setText(tStr);
}

void Player::showColorDialog()
{
    if (!m_colorDialog) {
        QSlider *brightnessSlider = new QSlider(Qt::Horizontal);
        brightnessSlider->setRange(-100, 100);
        brightnessSlider->setValue(m_videoWidget->brightness());
        brightnessSlider->setStyleSheet(Style::getSliderStyleSheet2());
        connect(brightnessSlider, &QSlider::sliderMoved, m_videoWidget, &QVideoWidget::setBrightness);
        connect(m_videoWidget, &QVideoWidget::brightnessChanged, brightnessSlider, &QSlider::setValue);

        QSlider *contrastSlider = new QSlider(Qt::Horizontal);
        contrastSlider->setRange(-100, 100);
        contrastSlider->setValue(m_videoWidget->contrast());
        contrastSlider->setStyleSheet(Style::getSliderStyleSheet2());
        connect(contrastSlider, &QSlider::sliderMoved, m_videoWidget, &QVideoWidget::setContrast);
        connect(m_videoWidget, &QVideoWidget::contrastChanged, contrastSlider, &QSlider::setValue);

        QSlider *hueSlider = new QSlider(Qt::Horizontal);
        hueSlider->setRange(-100, 100);
        hueSlider->setValue(m_videoWidget->hue());
        hueSlider->setStyleSheet(Style::getSliderStyleSheet2());
        connect(hueSlider, &QSlider::sliderMoved, m_videoWidget, &QVideoWidget::setHue);
        connect(m_videoWidget, &QVideoWidget::hueChanged, hueSlider, &QSlider::setValue);

        QSlider *saturationSlider = new QSlider(Qt::Horizontal);
        saturationSlider->setRange(-100, 100);
        saturationSlider->setValue(m_videoWidget->saturation());
        saturationSlider->setStyleSheet(Style::getSliderStyleSheet2());
        connect(saturationSlider, &QSlider::sliderMoved, m_videoWidget, &QVideoWidget::setSaturation);
        connect(m_videoWidget, &QVideoWidget::saturationChanged, saturationSlider, &QSlider::setValue);

        QFormLayout *layout = new QFormLayout;
        layout->addRow(tr("Brightness"), brightnessSlider);
        layout->addRow(tr("Contrast"), contrastSlider);
        layout->addRow(tr("Hue"), hueSlider);
        layout->addRow(tr("Saturation"), saturationSlider);

        QPushButton *button = new QPushButton(tr("OK"));
        button->setStyleSheet(Style::getBtnPushToolStyleSheet());
        layout->addRow(button);

        m_colorDialog = new QDialog(this);
        m_colorDialog->setWindowTitle(tr("Color Options"));
        m_colorDialog->setLayout(layout);

        connect(button, &QPushButton::clicked, m_colorDialog, &QDialog::close);
    }
    m_colorDialog->show();
}

void Player::clearHistogram()
{
    QMetaObject::invokeMethod(m_videoHistogram, "processFrame", Qt::QueuedConnection, Q_ARG(QVideoFrame, QVideoFrame()));
    QMetaObject::invokeMethod(m_audioHistogram, "processBuffer", Qt::QueuedConnection, Q_ARG(QAudioBuffer, QAudioBuffer()));
}

#ifdef WIN32
void Player::createTaskbar()
{
    m_taskbarButton = new QWinTaskbarButton(this);
    m_taskbarButton->setWindow(windowHandle());

    m_taskbarProgress = m_taskbarButton->progress();
    connect(m_slider, &QAbstractSlider::valueChanged, m_taskbarProgress, &QWinTaskbarProgress::setValue);
    connect(m_slider, &QAbstractSlider::rangeChanged, m_taskbarProgress, &QWinTaskbarProgress::setRange);

    connect(m_player, &QMediaPlayer::stateChanged, this, &Player::updateTaskbar);
}

void Player::updateTaskbar()
{
    switch (m_player->state()) {
    case QMediaPlayer::PlayingState:
        m_taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPlay));
        m_taskbarProgress->show();
        m_taskbarProgress->resume();
        break;
    case QMediaPlayer::PausedState:
        m_taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaPause));
        m_taskbarProgress->show();
        m_taskbarProgress->pause();
        break;
    case QMediaPlayer::StoppedState:
        m_taskbarButton->setOverlayIcon(style()->standardIcon(QStyle::SP_MediaStop));
        m_taskbarProgress->hide();
        break;
    }
}

void Player::createThumbnailToolBar()
{
    thumbnailToolBar = new QWinThumbnailToolBar(this);
    thumbnailToolBar->setWindow(windowHandle());

    playToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
    playToolButton->setEnabled(false);
    playToolButton->setToolTip(tr("Play"));
    playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    connect(playToolButton, &QWinThumbnailToolButton::clicked, this, &Player::togglePlayback);

    forwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
    forwardToolButton->setEnabled(false);
    forwardToolButton->setToolTip(tr("Fast forward"));
    forwardToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekForward));
    connect(forwardToolButton, &QWinThumbnailToolButton::clicked, this, &Player::seekForward);

    backwardToolButton = new QWinThumbnailToolButton(thumbnailToolBar);
    backwardToolButton->setEnabled(false);
    backwardToolButton->setToolTip(tr("Rewind"));
    backwardToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaSeekBackward));
    connect(backwardToolButton, &QWinThumbnailToolButton::clicked, this, &Player::seekBackward);

    thumbnailToolBar->addButton(backwardToolButton);
    thumbnailToolBar->addButton(playToolButton);
    thumbnailToolBar->addButton(forwardToolButton);

    connect(m_player, &QMediaPlayer::positionChanged, this, &Player::updateThumbnailToolBar);
    connect(m_player, &QMediaPlayer::durationChanged, this, &Player::updateThumbnailToolBar);
    connect(m_player, &QMediaPlayer::stateChanged, this, &Player::updateThumbnailToolBar);

}

void Player::updateThumbnailToolBar()
{
    playToolButton->setEnabled(m_player->duration() > 0);
    backwardToolButton->setEnabled(m_player->position() > 0);
    forwardToolButton->setEnabled(m_player->position() < m_player->duration());

    if (m_player->state() == QMediaPlayer::PlayingState) {
        playToolButton->setToolTip(tr("Pause"));
        playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
    } else {
        playToolButton->setToolTip(tr("Play"));
        playToolButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
    }
}

void Player::togglePlayback()
{
    if (m_player->state() == QMediaPlayer::PlayingState)
        m_player->pause();
    else
        m_player->play();
}

void Player::seekForward()
{
    m_slider->triggerAction(QSlider::SliderPageStepAdd);
}

void Player::seekBackward()
{
    m_slider->triggerAction(QSlider::SliderPageStepSub);
}
#endif
