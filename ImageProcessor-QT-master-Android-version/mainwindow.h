// http://www.amin-ahmadi.com

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QtWidgets>
#include <QtAndroidExtras>
#include <QPainter>
#include <QPixmap>
#include <QRubberBand>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void Label_current_pos();
    void Label_clicked();
    void Label_final_pos();

    void on_actionOpen_triggered();

    void on_actionSelect_triggered();

    void on_actionCrop_triggered();

    void on_actionRotate_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionReset_triggered();

    void on_actionSave_triggered();
    void saveAs();
    bool saveFile(const QString &fileName);

private:
    Ui::MainWindow *ui;
    bool show_tool_bar;
    QRubberBand* rubberBand;
};

#endif // MAINWINDOW_H
