#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QSpinBox>
#include <QMouseEvent>
#include <QRubberBand>
#include <QPoint>
#include <QStack>


QT_BEGIN_NAMESPACE
class QAction;
class QLabel;
class QMenu;
class QFrame;
class QSlider;
class QScrollArea;
class QScrollBar;
QT_END_NAMESPACE

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void mouseMoveEvent(QMouseEvent *ev);
    void mouseReleaseEvent(QMouseEvent *ev);
    void mousePressEvent(QMouseEvent *ev);
    // next integers for selecting corners' positions
    int start_x, start_y, end_x, end_y;
    bool selecting_mode, selected;
    QPoint origin;
    QRubberBand *rubberBand;

    struct data{
        QImage img;
        int angle;
    };

private slots:
    void open();
    void saveAs();
    void copy();
    void paste();
    void select();
    void crop();
    void zoom(int z);
    void normalSize();
    void fitToWindow(bool checked);
    void rotate(int angle);
    void reset();
    void on_fitToWindow_clicked(bool checked);
    void on_normal_clicked();
    void on_select_clicked();
    void on_zoomButton_clicked();
    void on_reset_clicked();
    void minimize();
    void maximize();
    void ex();

    void on_rotateDial_sliderReleased();

    void on_undoButton_clicked();

    void on_redoButton_clicked();

private:
    bool loadFile(const QString &);
    bool saveFile(const QString &fileName);
    void setImage(const QImage &newImage);
    void scaleImage(double factor);
    void adjustScrollBar(QScrollBar *scrollBar, double factor);
    void createMenu();
    void updateActions();
    void setZoomer();
    void setRotater();
    bool undo();
    bool redo();
    Ui::MainWindow *ui;
    QSpinBox *pen;
    QAction *save_as_act;
    QAction *copy_act;
    QAction *paste_act;
    QFrame *zoomPopUp;
    QSlider *zoomSlider;

    QImage image;
    QLabel *imageLabel;
    QScrollArea *scrollArea;
    double scaleFactor;
  private:
      QPoint oldPos;
      QStack<data> undo_stack;
      QStack<data> redo_stack;
};

#endif // MAINWINDOW_H
