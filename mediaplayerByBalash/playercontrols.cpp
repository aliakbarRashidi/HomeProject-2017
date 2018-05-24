#include "playercontrols.h"

#include <QBoxLayout>
#include <QSlider>
#include <QStyle>
#include <QToolButton>
#include <QComboBox>
#include <QAudio>

PlayerControls::PlayerControls(QWidget *parent)
    : QWidget(parent)
{
    m_playButton = new QToolButton(this);
    m_playButton->setStyleSheet(Style::getPlayStyleSheet());

    connect(m_playButton, &QAbstractButton::clicked, this, &PlayerControls::playClicked);

    m_stopButton = new QToolButton(this);
    m_stopButton->setStyleSheet(Style::getStopStyleSheet());
    m_stopButton->setEnabled(false);

    connect(m_stopButton, &QAbstractButton::clicked, this, &PlayerControls::stop);

    m_nextButton = new QToolButton(this);
    m_nextButton->setStyleSheet(Style::getNextStyleSheet());

    connect(m_nextButton, &QAbstractButton::clicked, this, &PlayerControls::next);

    m_previousButton = new QToolButton(this);
    m_previousButton->setStyleSheet(Style::getPreviousStyleSheet());

    connect(m_previousButton, &QAbstractButton::clicked, this, &PlayerControls::previous);

    m_muteButton = new QToolButton(this);
    m_muteButton->setStyleSheet("padding-left: 0px;"
                                "padding-right: 0px;"
                                "padding-top: 0px;"
                                "padding-bottom: 0px;");
    connect(m_muteButton, &QAbstractButton::clicked, this, &PlayerControls::muteClicked);

    m_volumeSlider = new QSlider(Qt::Horizontal, this);
    setStyleSheet(Style::getSliderStyleSheet2());
    m_volumeSlider->setRange(0, 100);

    connect(m_volumeSlider, &QSlider::valueChanged, this, &PlayerControls::onVolumeSliderValueChanged);

    m_nextButton->setToolTip("Next track");
    m_playButton->setToolTip("Play");
    m_previousButton->setToolTip("Previous");
    m_stopButton->setToolTip("Stop");

    m_rateBox = new QComboBox(this);
    m_rateBox->setStyleSheet(Style::getComboBoxStyleSheet());
    m_rateBox->addItem("0.5x", QVariant(0.5));
    m_rateBox->addItem("1.0x", QVariant(1.0));
    m_rateBox->addItem("2.0x", QVariant(2.0));
    m_rateBox->setCurrentIndex(1);

    connect(m_rateBox, QOverload<int>::of(&QComboBox::activated), this, &PlayerControls::updateRate);

    m_muteButton->setIcon(QIcon(":/buttons/volume_w.png"));
    m_muteButton->setStyleSheet("padding-left: 0px;"
                  "padding-right: 0px;"
                  "padding-top: 0px;"
                  "padding-bottom: 0px;");
    m_muteButton->setIconSize(QSize(20,20));

    QBoxLayout *layout = new QHBoxLayout;
    layout->setMargin(0);
    layout->addWidget(m_stopButton);
    layout->addWidget(m_previousButton);
    layout->addWidget(m_playButton);
    layout->addWidget(m_nextButton);
    layout->addWidget(m_muteButton);
    layout->addWidget(m_volumeSlider);
    layout->addWidget(m_rateBox);
    setLayout(layout);
}

QMediaPlayer::State PlayerControls::state() const
{
    return m_playerState;
}

void PlayerControls::setState(QMediaPlayer::State state)
{
    if (state != m_playerState) {
        m_playerState = state;

        switch (state) {
        case QMediaPlayer::StoppedState:
            m_stopButton->setEnabled(false);
            m_playButton->setStyleSheet(Style::getPlayStyleSheet());
            break;
        case QMediaPlayer::PlayingState:
            m_stopButton->setEnabled(true);
            m_playButton->setStyleSheet(Style::getPauseStyleSheet());
            break;
        case QMediaPlayer::PausedState:
            m_stopButton->setEnabled(true);
            m_playButton->setStyleSheet(Style::getPlayStyleSheet());
            break;
        }
    }
}

int PlayerControls::volume() const
{
    qreal linearVolume =  QAudio::convertVolume(m_volumeSlider->value() / qreal(100),
                                                QAudio::LogarithmicVolumeScale,
                                                QAudio::LinearVolumeScale);

    return qRound(linearVolume * 100);
}

void PlayerControls::setVolume(int volume)
{
    qreal logarithmicVolume = QAudio::convertVolume(volume / qreal(100),
                                                    QAudio::LinearVolumeScale,
                                                    QAudio::LogarithmicVolumeScale);

    m_volumeSlider->setValue(qRound(logarithmicVolume * 100));
}

bool PlayerControls::isMuted() const
{
    return m_playerMuted;
}

void PlayerControls::setMuted(bool muted)
{
    if (muted != m_playerMuted) {
        m_playerMuted = muted;

        m_muteButton->setIcon(style()->standardIcon(muted
                ? QStyle::SP_MediaVolumeMuted
                : QStyle::SP_MediaVolume));
    }
}

void PlayerControls::playClicked()
{
    switch (m_playerState) {
    case QMediaPlayer::StoppedState:
    case QMediaPlayer::PausedState:
        emit play();
        break;
    case QMediaPlayer::PlayingState:
        emit pause();
        break;
    }
}

void PlayerControls::muteClicked()
{
    emit changeMuting(!m_playerMuted);
}

qreal PlayerControls::playbackRate() const
{
    return m_rateBox->itemData(m_rateBox->currentIndex()).toDouble();
}

void PlayerControls::setPlaybackRate(float rate)
{
    for (int i = 0; i < m_rateBox->count(); ++i) {
        if (qFuzzyCompare(rate, float(m_rateBox->itemData(i).toDouble()))) {
            m_rateBox->setCurrentIndex(i);
            return;
        }
    }

    m_rateBox->addItem(QString("%1x").arg(rate), QVariant(rate));
    m_rateBox->setCurrentIndex(m_rateBox->count() - 1);
}

void PlayerControls::updateRate()
{
    emit changeRate(playbackRate());
}

void PlayerControls::onVolumeSliderValueChanged()
{
    emit changeVolume(volume());
}
