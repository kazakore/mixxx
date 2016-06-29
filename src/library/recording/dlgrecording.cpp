#include <QDesktopServices>

#include "control/controlobject.h"
#include "library/recording/dlgrecording.h"
#include "library/trackcollection.h"
#include "widget/wwidget.h"
#include "widget/wskincolor.h"
#include "widget/wtracktableview.h"
#include "util/assert.h"

DlgRecording::DlgRecording(QWidget* parent, TrackCollection* pTrackCollection,
                           RecordingManager* pRecordingManager)
        : QWidget(parent),
          m_pTrackCollection(pTrackCollection),
          m_browseModel(this, m_pTrackCollection, pRecordingManager),
          m_proxyModel(&m_browseModel),
          m_bytesRecordedStr("--"),
          m_durationRecordedStr("--:--"),
          m_pRecordingManager(pRecordingManager) {
    setupUi(this);

    connect(m_pRecordingManager, SIGNAL(isRecording(bool)),
            this, SLOT(slotRecordingEnabled(bool)));
    connect(m_pRecordingManager, SIGNAL(bytesRecorded(long)),
            this, SLOT(slotBytesRecorded(long)));
    connect(m_pRecordingManager, SIGNAL(durationRecorded(QString)),
            this, SLOT(slotDurationRecorded(QString)));

    m_recordingDir = m_pRecordingManager->getRecordingDir();

    m_proxyModel.setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel.setSortCaseSensitivity(Qt::CaseInsensitive);

    m_browseModel.setPath(m_recordingDir);

    connect(pushButtonRecording, SIGNAL(toggled(bool)),
            this,  SLOT(toggleRecording(bool)));
    label->setText("");
    label->setEnabled(false);
}

DlgRecording::~DlgRecording() {
}

void DlgRecording::onShow() {
    m_recordingDir = m_pRecordingManager->getRecordingDir();
    m_browseModel.setPath(m_recordingDir);
}

void DlgRecording::setTrackTable(WTrackTableView* pTrackTableView) {
    pTrackTableView->loadTrackModel(&m_proxyModel);
}

void DlgRecording::refreshBrowseModel() {
     m_browseModel.setPath(m_recordingDir);
}

void DlgRecording::toggleRecording(bool toggle) {
    Q_UNUSED(toggle);
    if (!m_pRecordingManager->isRecordingActive()) //If recording is enabled
    {
        //pushButtonRecording->setText(tr("Stop Recording"));
        m_pRecordingManager->startRecording();
    }
    else if(m_pRecordingManager->isRecordingActive()) //If we disable recording
    {
        //pushButtonRecording->setText(tr("Start Recording"));
        m_pRecordingManager->stopRecording();
    }
}

void DlgRecording::slotRecordingEnabled(bool isRecording) {
    if (isRecording) {
        pushButtonRecording->setText((tr("Stop Recording")));
        label->setEnabled(true);
    } else {
        pushButtonRecording->setText((tr("Start Recording")));
        label->setText("");
        label->setEnabled(false);
    }
    //This will update the recorded track table view
    m_browseModel.setPath(m_recordingDir);
}

// gets number of recorded bytes and update label
void DlgRecording::slotBytesRecorded(long bytes) {
    double megabytes = bytes / 1048576.0;
    m_bytesRecordedStr = QString::number(megabytes,'f',2);
    refreshLabel();
}

// gets recorded duration and update label
void DlgRecording::slotDurationRecorded(QString durationRecorded) {
    m_durationRecordedStr = durationRecorded;
    refreshLabel();
}

// update label besides start/stop button
void DlgRecording::refreshLabel() {
    QString text = tr("Recording to file: %1 (%2 MiB written in %3)")
              .arg(m_pRecordingManager->getRecordingFile())
              .arg(m_bytesRecordedStr)
              .arg(m_durationRecordedStr);
    label->setText(text);
 }
