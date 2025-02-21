#include "gotodialog.h"
#include "ui_gotodialog.h"

#include "mapview.h"

#include <QDoubleValidator>
#include <QKeyEvent>
#include <QClipboard>
#include <QMessageBox>

static bool g_animate;

GotoDialog::GotoDialog(MapView *map, qreal x, qreal z, qreal scale)
    : QDialog(map)
    , ui(new Ui::GotoDialog)
    , mapview(map)
{
    ui->setupUi(this);

    scalemin = 1.0 / 4096;
    scalemax = 65536;
    ui->lineX->setValidator(new QDoubleValidator(-3e7, 3e7, 1, ui->lineX));
    ui->lineZ->setValidator(new QDoubleValidator(-3e7, 3e7, 1, ui->lineZ));
    ui->lineScale->setValidator(new QDoubleValidator(scalemin, scalemax, 16, ui->lineScale));

    ui->lineX->setText(QString::asprintf("%.1f", x));
    ui->lineZ->setText(QString::asprintf("%.1f", z));
    ui->lineScale->setText(QString::asprintf("%.4f", scale));

    ui->checkAnimate->setChecked(g_animate);
}

GotoDialog::~GotoDialog()
{
    delete ui;
}

void GotoDialog::on_buttonBox_clicked(QAbstractButton *button)
{
    QDialogButtonBox::StandardButton b = ui->buttonBox->standardButton(button);

    if (b == QDialogButtonBox::Ok || b == QDialogButtonBox::Apply)
    {
        qreal x = ui->lineX->text().toDouble();
        qreal z = ui->lineZ->text().toDouble();
        qreal scale = ui->lineScale->text().toDouble();
        if (scale > 4096)
        {
            int button = QMessageBox::warning(this, tr("缩放比例过大"),
                tr("将缩放比例调得过大可能导致一系列问题\n"
                   "是否继续？"), QMessageBox::Abort|QMessageBox::Yes);
            if (button == QMessageBox::Abort)
                return;
        }
        if (scale < scalemin) scale = scalemin;
        if (scale > scalemax) scale = scalemax;
        ui->lineScale->setText(QString::asprintf("%.4f", scale));
        g_animate = ui->checkAnimate->isChecked();
        if (g_animate)
            mapview->animateView(x, z, scale);
        else
            mapview->setView(x, z, scale);
    }
    else if (b == QDialogButtonBox::Reset)
    {
        ui->lineX->setText("0");
        ui->lineZ->setText("0");
        ui->lineScale->setText("16");
    }
}

void GotoDialog::keyReleaseEvent(QKeyEvent *event)
{
    if (event->matches(QKeySequence::Paste))
    {
        QClipboard *clipboard = QGuiApplication::clipboard();
        QString s = clipboard->text().trimmed();
        QStringList xz = s.split(QRegExp("[, ]+"));
        if (xz.count() == 2)
        {
            ui->lineX->setText(xz[0]);
            ui->lineZ->setText(xz[1]);
            return;
        }
        else if (xz.count() == 3)
        {
            ui->lineX->setText(xz[0]);
            ui->lineZ->setText(xz[2]);
            return;
        }
    }
    QWidget::keyReleaseEvent(event);
}

