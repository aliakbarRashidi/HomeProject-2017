#include "volumebutton.h"

VolumeButton::VolumeButton(QWidget *parent):
    QToolButton(parent)
{
    setPopupMode(QToolButton::InstantPopup);

    setIcon(QIcon(":/buttons/volume_w.png"));
    setStyleSheet("padding-left: 0px;"
                  "padding-right: 0px;"
                  "padding-top: 0px;"
                  "padding-bottom: 0px;");
    setIconSize(QSize(20,20));
    QWidget *popup = new QWidget(this);

    m_slider = new QSlider(Qt::Horizontal, popup);
    m_slider->setRange(0,100);
    m_slider->setStyleSheet(Style::getSliderStyleSheet2());

    connect(m_slider, &QAbstractSlider::valueChanged, this, &VolumeButton::volumeChanged);

    m_label = new QLabel(popup);
    m_label->setAlignment(Qt::AlignCenter);
    m_label->setNum(100);
    m_label->setMinimumWidth(m_label->sizeHint().width());

    typedef void(QLabel::*IntSlot)(int);
    connect(m_slider, &QAbstractSlider::valueChanged, m_label, static_cast<IntSlot>(&QLabel::setNum));

    QBoxLayout *popupLayout = new QHBoxLayout(popup);
    popupLayout->setMargin(1);
    popupLayout->addWidget(m_slider);
    popupLayout->addWidget(m_label);


    QWidgetAction *action = new QWidgetAction(this);
    action->setDefaultWidget(popup);

    m_menu = new QMenu(this);
    m_menu->addAction(action);
    setMenu(m_menu);
}

void VolumeButton::increaseVolume()
{
    m_slider->triggerAction(QSlider::SliderPageStepAdd);
}

void VolumeButton::descreaseVolume()
{
    m_slider->triggerAction(QSlider::SliderPageStepSub);
}

int VolumeButton::volume() const
{
    return m_slider->value();
}

void VolumeButton::setVolume(int volume)
{
    m_slider->setValue(volume);
}
