#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtWidgets>

QString fName;
bool fitToWindowAct;
int prevZoom = 50;
int currentAngle = 0;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    imageLabel(new QLabel)
    ,ui(new Ui::MainWindow)
  , scrollArea(new QScrollArea)
    , scaleFactor(1)
{
    ui->setupUi(this);
    setWindowFlags(Qt::FramelessWindowHint);
    //ui->mainToolBar->close();
    ui->widget->show();
    ui->mainToolBar->addWidget(ui->widget);
    ui->mainToolBar->show();
    //setFixedSize(1150,700);
    // zoom
    setZoomer();
    // rotate
    setRotater();
    // crop
    connect(ui->crop,SIGNAL(clicked()),this,SLOT(crop()));
    // imagelabel

    scrollArea=  ui->scrollArea;
    imageLabel->setBackgroundRole(QPalette::Base);
    imageLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    imageLabel->setScaledContents(true);
    scrollArea->setBackgroundRole(QPalette::Dark);
    scrollArea->setWidget(imageLabel);
    scrollArea->setVisible(false);
    setCentralWidget(scrollArea);
    createMenu();
    resize(QGuiApplication::primaryScreen()->availableSize() * 3 / 5);
    this->rubberBand = new QRubberBand(QRubberBand::Rectangle, this);
    this->selected = false;
    this->selecting_mode = false;
    ui->crop->setEnabled(selected);
}

void MainWindow::setZoomer(){
    zoomPopUp = new QFrame( this, Qt::Popup);
    zoomPopUp->resize(20,150);

    zoomSlider = new QSlider(Qt::Vertical, zoomPopUp);
    zoomSlider->resize(20,150);
    zoomSlider->setRange(0, 100);
    zoomSlider->setTickPosition(QSlider::TicksLeft);
    zoomSlider->setTickInterval(10);
    zoomSlider->setSingleStep(5);
    zoomSlider->setStyleSheet(
               "QSlider::handle:vertical {"
               "height: 10px;"
               "width: 18px;"
               "background-color: rgb(59,98,231);"
               "border: 1px solid transparent;"
               "border-radius: 5px;}");
    zoomSlider->show();
    zoomSlider->setValue(50);
    connect(zoomSlider,SIGNAL(valueChanged(int)), this,SLOT(zoom(int)));
}

void MainWindow::setRotater(){
      ui->rotateDial->setRange(-360,360);
      ui->rotateDial->setValue(0);
      ui->rotateDial->setFocusPolicy(Qt::StrongFocus);;
      ui->rotateDial->setSingleStep(1);
      connect(ui->rotateDial, SIGNAL(valueChanged(int)), this, SLOT(rotate(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::createMenu(){

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    QAction *openAct = fileMenu->addAction(tr("&Open..."), this, &MainWindow::open);
    openAct->setShortcut(QKeySequence::Open);

    save_as_act = fileMenu->addAction(tr("&Save As..."), this, &MainWindow::saveAs);
    save_as_act->setShortcut(tr("Ctrl+S"));
    save_as_act->setEnabled(false);

    fileMenu->addSeparator();

    QAction *exitAct = fileMenu->addAction(tr("&Exit"), this, &QWidget::close);
    exitAct->setShortcut(tr("Ctrl+Q"));

    QMenu *editMenu = menuBar()->addMenu(tr("&Edit"));
    copy_act = editMenu->addAction(tr("&Copy"), this, &MainWindow::copy);
    copy_act->setShortcut(QKeySequence::Copy);
    copy_act->setEnabled(false);

    QAction *pasteAct = editMenu->addAction(tr("&Paste"), this, &MainWindow::paste);
    pasteAct->setShortcut(QKeySequence::Paste);

    QWidget * wdg = new QWidget(this);
    QHBoxLayout *hlay = new QHBoxLayout(wdg);
    QPushButton* min = new QPushButton(" _ ", wdg);
    min->setStyleSheet("background-color:transparent;");
    connect(min,SIGNAL(clicked()),this,SLOT(minimize()));
    hlay->addWidget(min);
    QPushButton* max = new QPushButton(" ◻ ", wdg);
     max->setStyleSheet("background-color:transparent;");
    connect(max,SIGNAL(clicked()),this,SLOT(maximize()));
    hlay->addWidget(max);
    QPushButton* ex = new QPushButton(" X ", wdg);
    ex->setStyleSheet("background-color:transparent;");
    connect(ex,SIGNAL(clicked()),this,SLOT(ex()));
    hlay->addWidget(ex);
    wdg->setLayout(hlay);
    menuBar()->setCornerWidget(wdg, Qt::TopRightCorner);

    selecting_mode = false;
}

void MainWindow::minimize(){
    setWindowState(Qt::WindowMinimized);
}

void MainWindow::maximize(){
   setWindowState(windowState() ^ Qt::WindowFullScreen);
}

void MainWindow::ex(){
    this->close();
}

void MainWindow::updateActions()
{
    save_as_act->setEnabled(!image.isNull());
    copy_act->setEnabled(!image.isNull());
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

// load file
void MainWindow::open(){
    QFileDialog dialog(this, tr("Open File"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptOpen);

    while (dialog.exec() == QDialog::Accepted && !loadFile(dialog.selectedFiles().first())){}
    updateActions();
}

bool MainWindow::loadFile(const QString &fileName){

    QImageReader reader(fileName);
    reader.setAutoTransform(true);
    const QImage newImage = reader.read();
    if (newImage.isNull()) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot load %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName), reader.errorString()));
        return false;
    }
    setImage(newImage);
    setWindowFilePath(fileName);
    fName = fileName;
    const QString message = tr("Opened \"%1\", %2x%3, Depth: %4")
        .arg(QDir::toNativeSeparators(fileName)).arg(image.width()).arg(image.height()).arg(image.depth());
    statusBar()->showMessage(message);
    return true;
}


void MainWindow::setImage(const QImage &newImage){
    data temp ;
    temp.img = newImage.copy();
    temp.angle = 0;
    undo_stack.push(temp);
    image = newImage;
    imageLabel->setPixmap(QPixmap::fromImage(image));
    scaleFactor = 1.0;
    scrollArea->setVisible(true);
    updateActions();
    fitToWindow(false);
}


// save as

void MainWindow::saveAs(){

    QFileDialog dialog(this, tr("Save File As"));
    initializeImageFileDialog(dialog, QFileDialog::AcceptSave);

    while (dialog.exec() == QDialog::Accepted && !saveFile(dialog.selectedFiles().first())) {}

}

bool MainWindow::saveFile(const QString &fileName)
{
    QImageWriter writer(fileName);

    if (!writer.write(image)) {
        QMessageBox::information(this, QGuiApplication::applicationDisplayName(),
                                 tr("Cannot write %1: %2")
                                 .arg(QDir::toNativeSeparators(fileName)), writer.errorString());
        return false;
    }
    const QString message = tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(fileName));
    statusBar()->showMessage(message);
    return true;
}


void MainWindow::copy()
{
#ifndef QT_NO_CLIPBOARD
    QGuiApplication::clipboard()->setImage(image);
#endif // !QT_NO_CLIPBOARD
}

#ifndef QT_NO_CLIPBOARD
static QImage clipboardImage()
{
    if (const QMimeData *mimeData = QGuiApplication::clipboard()->mimeData()) {
        if (mimeData->hasImage()) {
            const QImage image = qvariant_cast<QImage>(mimeData->imageData());
            if (!image.isNull())
                return image;
        }
    }
    return QImage();
}
#endif // !QT_NO_CLIPBOARD

void MainWindow::paste()
{
#ifndef QT_NO_CLIPBOARD
    const QImage newImage = clipboardImage();
    if (newImage.isNull()) {
        statusBar()->showMessage(tr("No image in clipboard"));
    } else {
        setImage(newImage);
        setWindowFilePath(QString());
        const QString message = tr("Obtained image from clipboard, %1x%2, Depth: %3")
            .arg(newImage.width()).arg(newImage.height()).arg(newImage.depth());
        statusBar()->showMessage(message);
    }
#endif // !QT_NO_CLIPBOARD
}

void MainWindow::select(){
    this->setCursor(Qt::CrossCursor);
    this->selecting_mode = true;
}

void MainWindow::crop(){
    qInfo("xs:%d ,, ys:%d ,, ex:%d ,, ey:%d",origin.x(), origin.y(), end_x-origin.x(), end_y-origin.y());
    QImage new_image = image.copy(origin.x(), origin.y(), end_x-origin.x(), end_y-origin.y());
    image = new_image;
    setImage(image.copy());
    selected = false;
    ui->crop->setEnabled(this->selected);
    rubberBand->hide();

}

void MainWindow::zoom(int z){

    if(z>prevZoom){
       scaleImage(1.25);
    }
    else
        scaleImage(.8);
     prevZoom = z;
}
void MainWindow::normalSize(){
    imageLabel->adjustSize();
    scaleFactor = 1.0;
}
void MainWindow::fitToWindow(bool fitted){
    scrollArea->setWidgetResizable(fitted);
    if (!fitted)
        normalSize();
    updateActions();
}

void MainWindow::rotate(int angle){
    QApplication::processEvents();
    QPixmap ship(fName);
    QPixmap rotate(ship.size());
    QPainter p(&rotate);
    p.setRenderHint(QPainter::Antialiasing);
    p.setRenderHint(QPainter::SmoothPixmapTransform);
    p.setRenderHint(QPainter::HighQualityAntialiasing);
    p.translate(rotate.size().width() / 2, rotate.size().height() / 2);
    p.rotate(angle);
    p.translate(-rotate.size().width() / 2, -rotate.size().height() / 2);
    p.drawPixmap(0, 0, ship);
    p.end();
    imageLabel->setPixmap(rotate);
    currentAngle = angle;

    //MainWindow::fitToWindow(true);
}

void MainWindow::scaleImage(double factor)
{
    if(imageLabel != NULL && (scaleFactor< 3 || scaleFactor > 0.333)){
        Q_ASSERT(imageLabel->pixmap());
      //  scaleFactor = 1 + (2*(factor-50)/100.0)*scaleFactor;
        //scaleFactor*=1.25;
        scaleFactor *= factor;
        imageLabel->resize(scaleFactor * imageLabel->pixmap()->size());

        if(this->selected){
            origin = QPoint(origin.x()*factor, origin.y()*factor);
            end_x = end_x*factor;
            end_y = end_y*factor;
            rubberBand->setGeometry(QRect(origin, QPoint(end_x, end_y)).normalized());
            int Hvalue = scrollArea->horizontalScrollBar()->value();
            int Vvalue = scrollArea->verticalScrollBar()->value();

            while(origin.x()>this->width()/2){
                int hLen = scrollArea->horizontalScrollBar()->maximum()-scrollArea->horizontalScrollBar()->minimum()+scrollArea->horizontalScrollBar()->pageStep();
                Hvalue = scrollArea->horizontalScrollBar()->value();
                scrollArea->horizontalScrollBar()->setValue(Hvalue+100);
                double leftRatio = 100/double(hLen) ;
                end_x = end_x-leftRatio*origin.x();
                origin.setX(origin.x()-leftRatio*origin.x());
                rubberBand->setGeometry(QRect(origin, QPoint(end_x, end_y)).normalized());
            }

            while(origin.y()>this->height()/2){
                int vLen = scrollArea->verticalScrollBar()->maximum()-scrollArea->verticalScrollBar()->minimum()+scrollArea->verticalScrollBar()->pageStep();
                Vvalue = scrollArea->verticalScrollBar()->value();
                scrollArea->verticalScrollBar()->setValue(Vvalue+100);
                double downRatio = 100/double(vLen) ;
                end_y = end_y-downRatio*origin.y();
                origin.setY(origin.y()-downRatio*origin.y());
                rubberBand->setGeometry(QRect(origin, QPoint(end_x, end_y)).normalized());
            }
        }else{
            scrollArea->horizontalScrollBar()->setValue(
                        int(factor * scrollArea->horizontalScrollBar()->value()));
            scrollArea->verticalScrollBar()->setValue(
                        int(factor * scrollArea->verticalScrollBar()->value()));
        }
    }
}

void MainWindow::reset(){
    qInfo("nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn");
    MainWindow::loadFile(fName);
}


void MainWindow::adjustScrollBar(QScrollBar *scrollBar, double factor)
{
    scrollBar->setValue(int(factor * scrollBar->value()
                            + ((factor - 1) * scrollBar->pageStep()/2)));
}



void MainWindow::on_zoomButton_clicked()
{
    //connect(zoomSlider,SIGNAL(valueChanged(int)), this,SLOT(zoom(int)));
    zoomSlider->setValue(50);
    zoomPopUp->move(mapToGlobal(ui->zoomButton->geometry().topRight()));
    zoomPopUp->show();

}
void MainWindow::on_fitToWindow_clicked(bool checked)
{
    MainWindow::fitToWindow(checked);
}

void MainWindow::on_normal_clicked()
{
    MainWindow::normalSize();
}

void MainWindow::on_select_clicked()
{
    MainWindow::select();
}



void MainWindow::on_reset_clicked()
{
     MainWindow::reset();
}



void MainWindow::mousePressEvent(QMouseEvent *ev){
    oldPos = ev->globalPos();
    if(this->selecting_mode){
        origin = ev->pos();
        rubberBand->setGeometry(QRect(origin, QSize()));
        rubberBand->show();
        this->start_x = ev->x();
        this->start_y = ev->y();
    }else{
        rubberBand->hide();
        this->selected = false;
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *ev){
    if(this->selecting_mode)
        rubberBand->setGeometry(QRect(origin, ev->pos()).normalized());
    else{
        const QPoint delta = ev->globalPos() - oldPos;
        move(x()+delta.x(), y()+delta.y());
        oldPos = ev->globalPos();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *ev){
    this->end_x = ev->x();
    this->end_y = ev->y();
    if(this->selecting_mode)
        this->selected = true;
    else
        this->selected = false;
    ui->crop->setEnabled(this->selected);
    this->selecting_mode = false;
    this->setCursor(Qt::ArrowCursor);
}

bool MainWindow::undo(){
    if(undo_stack.size()<=1){
            return false;
     }
        redo_stack.push(undo_stack.pop());
        data temp = undo_stack.top();
        setImage(temp.img);
        undo_stack.pop();
        rotate(temp.angle);
        return true;
}
bool MainWindow::redo(){
    if(redo_stack.empty()){
        return false;
    }
    data temp = redo_stack.pop();
    setImage(temp.img);
    rotate(temp.angle);
    return true;
}

void MainWindow::on_rotateDial_sliderReleased()
{
    data temp;
    temp.img = image.copy();
    temp.angle = currentAngle;
    undo_stack.push(temp);
}

void MainWindow::on_undoButton_clicked()
{
    MainWindow::undo();
    qInfo("undo: %d ,redo: %d",undo_stack.size(),redo_stack.size());
}

void MainWindow::on_redoButton_clicked()
{
    MainWindow::redo();
    qInfo("undo: %d ,redo: %d",undo_stack.size(),redo_stack.size());
}
