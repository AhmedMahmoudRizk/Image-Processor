// http://www.amin-ahmadi.com

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "clickablelabel.h"


QString selectedFileName;

#ifdef __cplusplus
extern "C" {
#endif

JNIEXPORT void JNICALL
Java_com_amin_QtAndroidGallery_QtAndroidGallery_fileSelected(JNIEnv */*env*/,
                                                             jobject /*obj*/,
                                                             jstring results)
{
    selectedFileName = QAndroidJniObject(results).toString();
}

#ifdef __cplusplus
}
#endif


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
  ,ui(new Ui::MainWindow)
{
    ui->setupUi(this);
       connect(ui->image , SIGNAL(clicked()) , this , SLOT(Label_clicked()));
       connect(ui->image , SIGNAL(moved()) , this , SLOT(Label_current_pos()));
       connect(ui->image , SIGNAL(released()) , this , SLOT(Label_final_pos()));
       ui->toolBar->toggleViewAction()->setChecked(true);
       ui->toolBar_2->toggleViewAction()->setChecked(true);
       show_tool_bar = true;
       QList<Qt::GestureType> gestures;
           gestures << Qt::PinchGesture;
           gestures << Qt::PanGesture;
       ui->image->grabGestures(gestures);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::Label_current_pos()
{

}

void MainWindow::Label_clicked()
{
    if(ui->image->pixmap()!=0 && !ui->image->selected){
        show_tool_bar = !show_tool_bar;
        ui->toolBar->toggleViewAction()->trigger();
        ui->toolBar_2->toggleViewAction()->trigger();
        if(!show_tool_bar)
        {
            ui->image->setGeometry(this->rect());
    }else{
        ui->image->setGeometry(ui->image->defaultRect);
    }
    }
}

void MainWindow::Label_final_pos()
{

}


void MainWindow::on_actionRedo_triggered()
{
    if(ui->image->horizontalOffset2.size()>0){
        ui->image->Reset();
        ui->image->currentImage=ui->image->currentImage2.pop();
        ui->image->setGeometry(ui->image->labelRec2.pop());
        ui->image->horizontalOffset=ui->image->horizontalOffset2.pop();
        ui->image->verticalOffset=ui->image->verticalOffset2.pop();
        ui->image->rotationAngle=ui->image->rotationAngle2.pop();
        ui->image->scaleFactor=ui->image->scaleFactor2.pop();
        ui->image->currentStepScaleFactor=ui->image->currentStepScaleFactor2.pop();

        ui->image->currentImage1.push(ui->image->currentImage);
        ui->image->labelRec1.push(ui->image->defaultRect);
        ui->image->horizontalOffset1.push(ui->image->horizontalOffset);
        ui->image->verticalOffset1.push(ui->image->verticalOffset);
        ui->image->rotationAngle1.push(ui->image->rotationAngle);
        ui->image->scaleFactor1.push(ui->image->scaleFactor);
        ui->image->currentStepScaleFactor1.push(ui->image->currentStepScaleFactor);

    }
}

void MainWindow::on_actionUndo_triggered()
{
    if(ui->image->horizontalOffset1.size()>0){
        ui->image->Reset();
        ui->image->currentImage=ui->image->currentImage1.pop();
        ui->image->setGeometry(ui->image->labelRec1.pop());
        ui->image->horizontalOffset=ui->image->horizontalOffset1.pop();
        ui->image->verticalOffset=ui->image->verticalOffset1.pop();
        ui->image->rotationAngle=ui->image->rotationAngle1.pop();
        ui->image->scaleFactor=ui->image->scaleFactor1.pop();
        ui->image->currentStepScaleFactor=ui->image->currentStepScaleFactor1.pop();


        ui->image->currentImage2.push(ui->image->currentImage);
        ui->image->labelRec2.push(ui->image->defaultRect);
        ui->image->horizontalOffset2.push(ui->image->horizontalOffset);
        ui->image->verticalOffset2.push(ui->image->verticalOffset);
        ui->image->rotationAngle2.push(ui->image->rotationAngle);
        ui->image->scaleFactor2.push(ui->image->scaleFactor);
        ui->image->currentStepScaleFactor2.push(ui->image->currentStepScaleFactor);

    }
}

void MainWindow::on_actionOpen_triggered()
{
    if(ui->image->defaultRect.isEmpty()){
        ui->image->defaultRect = ui->image->rect();
    }
    selectedFileName = "#";
    QAndroidJniObject::callStaticMethod<void>("com/amin/QtAndroidGallery/QtAndroidGallery",
                                              "openAnImage",
                                              "()V");
    while(selectedFileName == "#")
        qApp->processEvents();

    if(QFile(selectedFileName).exists())
    {

        ui->image->Reset();
        ui->image->setGeometry(ui->image->defaultRect);
        QImage img(selectedFileName);
        img = img.scaled(ui->image->width(),ui->image->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        ui->image->setPixmap(QPixmap::fromImage(img));
        ui->image->currentImage = img;
        ui->image->addToStack();
    }
}

void MainWindow::on_actionSelect_triggered()
{
    ui->image->selected = true;
}

void MainWindow::on_actionCrop_triggered()
{
    if(ui->image->selected) //handle crop when no
    {
        ui->image->selected = false;
        qInfo() << "released";
        ui->image->rubberBand->hide();
        rubberBand = ui->image->rubberBand;
        QPoint topLeft(rubberBand->x(),rubberBand->y());
        QPoint bottomRight(rubberBand->width()+rubberBand->x(),rubberBand->height()+rubberBand->y());
        QRect myRect(topLeft,bottomRight);  //rect of rubberband is zero centered we should add start offset

        QPixmap OriginalPix(*ui->image->pixmap());

        QImage newImage;
        newImage = OriginalPix.toImage();

        QImage copyImage;
        copyImage = newImage.copy(myRect);

        copyImage = copyImage.scaled(ui->image->width(),ui->image->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        ui->image->currentImage = copyImage;
        ui->image->setPixmap(QPixmap::fromImage(copyImage));
        ui->image->addToStack();
        ui->image->repaint();

    }
}

void MainWindow::on_actionRotate_triggered()
{
    ui->image->rotate = true;
}

void MainWindow::on_actionReset_triggered()
{
    if(QFile(selectedFileName).exists())
    {
        ui->image->Reset();
        ui->image->setGeometry(ui->image->defaultRect);
        QImage img(selectedFileName);
        img = img.scaled(ui->image->width(),ui->image->height(),Qt::KeepAspectRatio,Qt::SmoothTransformation);
        ui->image->setPixmap(QPixmap::fromImage(img));
        ui->image->currentImage = img;
        ui->image->addToStack();
      }
}
static void initializeImageFileDialog(QFileDialog &dialog, QFileDialog::AcceptMode acceptMode)
{
    static bool firstDialog = true;

    if (firstDialog) {
        firstDialog = false;
        const QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
        dialog.setDirectory(picturesLocations.isEmpty() ? QDir::currentPath() : picturesLocations.last());
    }

    QStringList mimeTypeFilters;
    const QByteArrayList supportedMimeTypes = acceptMode == QFileDialog::AcceptOpen
        ? QImageReader::supportedMimeTypes() : QImageWriter::supportedMimeTypes();
    foreach (const QByteArray &mimeTypeName, supportedMimeTypes)
        mimeTypeFilters.append(mimeTypeName);
        mimeTypeFilters.sort();
        dialog.setMimeTypeFilters(mimeTypeFilters);
        dialog.selectMimeTypeFilter("image/jpeg");
        if (acceptMode == QFileDialog::AcceptSave)
            dialog.setDefaultSuffix("jpg");
}
void MainWindow::saveAs(){

    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}

}

bool MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(ui->image->currentImage)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}
void MainWindow::on_actionSave_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save File"),
                                       "",
                                       tr("Images (*.png *.xpm *.jpg)"));
           if(!ui->image->currentImage.isNull() && !fileName.isNull() && !fileName.isEmpty()){
               ui->image->currentImage.save(fileName);
           }else{

               QMessageBox::warning(this, tr("Image_editor"),
                                              tr("Error while saving!"));
           }
}
