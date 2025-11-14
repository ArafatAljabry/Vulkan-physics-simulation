#ifndef HELLOVULKANWIDGET_H
#define HELLOVULKANWIDGET_H

#include <QWidget>

QT_FORWARD_DECLARE_CLASS(QTabWidget)
QT_FORWARD_DECLARE_CLASS(QPlainTextEdit)

//Forward declaration
class VulkanWindow;
class QMenu;
class QMenuBar;
class QAction;
class QDialogButtonBox;

//The class that holds the whole GUI of the application
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(VulkanWindow *vw, QPlainTextEdit *logWidget);

public slots:
    void onScreenGrabRequested();

private:
    VulkanWindow *mVulkanWindow{ nullptr };
    QTabWidget *mInfoTab{ nullptr };
    QPlainTextEdit *mLogWidget{ nullptr };

    QMenuBar* createMenu();

    QMenuBar* menuBar{ nullptr };
    QMenu* fileMenu{ nullptr };
    QAction* openFileAction{ nullptr };
    QAction* exitAction{ nullptr };

    //NOTE: button to restart the application
    QAction* restartAction{nullptr};
    std::string mSelectedName;

private slots:
    void openFile();
    void selectName();

    //NOTE: defines the function that restarts the game
    void Restart();
};

#endif // HELLOVULKANWIDGET_H
