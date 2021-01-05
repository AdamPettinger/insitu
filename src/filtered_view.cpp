#include "filtered_view.hpp"

namespace insitu {

FilteredView::FilteredView(QString _topic, QWidget * parent) : QWidget(parent)
{
    // Topic name selector
    topicBox = new QComboBox();
    topicBox->addItems(getTopicList());
    int idx = topicBox->findText(_topic);
    topicBox->setCurrentIndex(idx);
    onTopicChange(topicBox->currentText());

    // buttons
    refreshTopicButton = new QPushButton(tr("Refresh"));
    addFilterButton = new QPushButton(tr("Add Filter"));

    // frame to preserve image aspect ratio
    imgFrame = new QFrame();
    imgFrame->setFrameStyle(QFrame::Plain | QFrame::Box);
    imgFrame->setBackgroundRole(QPalette::Base);

    // label to hold image
    imgLabel = new QLabel(imgFrame);
    imgLabel->setBackgroundRole(QPalette::Base);
    imgLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
    imgLabel->setScaledContents(true);

    // display statistics in lower status bar
    fpsLabel = new QLabel();
    fpsLabel->setText(tr("FPS: "));

    // panel for filters
    filterList = new QListWidget();

    // layout
    layout = new QGridLayout();
    layout->addWidget(topicBox, 0, 0);
    layout->addWidget(refreshTopicButton, 0, 1);
    layout->addWidget(addFilterButton, 0, 2);
    layout->addWidget(imgFrame, 1, 0, 1, 2);
    layout->addWidget(filterList, 1, 2);
    layout->addWidget(fpsLabel, 2, 0);

    layout->setColumnStretch(0, 1);

    lastFrameTime = ros::Time::now();
    frames = 0;

    setLayout(layout);

    QObject::connect(topicBox, SIGNAL(currentIndexChanged(const QString&)),
                     SLOT(onTopicChange(const QString&)));
    QObject::connect(refreshTopicButton, SIGNAL(clicked()),
                     SLOT(refreshTopics()));
    QObject::connect(addFilterButton, SIGNAL(clicked()),
                     SLOT(addFilter()));
}

FilteredView::~FilteredView(void)
{
    sub.shutdown();
}

void FilteredView::refreshTopics(void)
{
    QString save_topic = topicBox->currentText();
    topicBox->clear();
    topicBox->addItems(getTopicList());
    int idx = topicBox->findText(save_topic);
    idx = idx == -1 ? 0 : idx;
    topicBox->setCurrentIndex(idx);
}

void FilteredView::addFilter(void)
{
    AddFilterDialog * afd = (AddFilterDialog *) getNamedWidget("addfilterdialog");
    afd->open();
}

void FilteredView::onTopicChange(QString topic_transport)
{
    static bool init = true;

    // imgMat.release();

    QList<QString> l = topic_transport.split(" ");
    std::string topic = l.first().toStdString();
    std::string transport = l.length() == 2 ? l.last().toStdString() : "raw";

    if (!topic.empty()) {
        if (init) init = false;
        else sub.shutdown();
        
        image_transport::ImageTransport it(nh);
        image_transport::TransportHints hints(transport);

        sub = it.subscribe(topic, 1, &FilteredView::callbackImg, this, hints);
    } else {
        // TODO error message
    }

    // qDebug("Subscribed to topic %s / %s", sub.getTopic().c_str(), sub.getTransport().c_str());
}

const QSize hacky_shit(2,2);

void FilteredView::callbackImg(const sensor_msgs::Image::ConstPtr& msg)
{
    // track frames per second
    ros::Time now = ros::Time::now();
    ++frames;
    if (now - lastFrameTime > ros::Duration(1)) {
        fpsLabel->setText(QString("FPS: %1").arg(frames));
        frames = 0;
        lastFrameTime = now;
    }

    // display image
    cv_bridge::CvImageConstPtr cv_ptr;
    try {
        cv_ptr = cv_bridge::toCvShare(msg, sensor_msgs::image_encodings::RGB8);
    } catch (cv_bridge::Exception& e) {
        qWarning("Failed to convert image: %s", e.what());
        return;
    }

    imgMat = cv_ptr->image;
    QImage image(imgMat.data, imgMat.cols, imgMat.rows, imgMat.step[0], QImage::Format_RGB888);
    QPixmap img = QPixmap::fromImage(image);

    // maximize while preserving aspect ratio
    QSize s = img.size();
    s.scale(imgFrame->size() - hacky_shit, Qt::KeepAspectRatio);
    imgLabel->resize(s);
    imgLabel->setPixmap(img);
}

}