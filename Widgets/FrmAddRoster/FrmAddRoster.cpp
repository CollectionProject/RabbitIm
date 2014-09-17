#include "FrmAddRoster.h"
#include "ui_FrmAddRoster.h"
#include "qxmpp/QXmppRosterManager.h"
#include "qxmpp/QXmppUtils.h"
#include "../../Global/Global.h"
#include <string>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopWidget>

CFrmAddRoster::CFrmAddRoster(QWidget *parent) :
    QFrame(parent),
    ui(new Ui::CFrmAddRoster)
{
    ui->setupUi(this);
    ui->txtGroup->setEditable(true);

    m_bRequest = false;

    QDesktopWidget *pDesk = QApplication::desktop();
    move((pDesk->width() - width()) / 2,
         (pDesk->height() - height()) / 2);
}

CFrmAddRoster::~CFrmAddRoster()
{
    delete ui;
}

int CFrmAddRoster::Init(QSet<QString> groups, QString szId)
{
    ui->txtJID->clear();
    ui->txtNick->clear();
    ui->txtGroup->clear();
    if(!szId.isEmpty())
    {
        this->setWindowTitle(tr("Request add roster"));
        ui->lbPrompt->setText(tr("%1 request add roster").arg(QXmppUtils::jidToUser(szId)));
        ui->txtJID->setEnabled(false);
        ui->txtJID->setText(szId);
        on_txtJID_editingFinished();
        m_bRequest = true;
    }
    else
    {
        this->setWindowTitle(tr("Add Roster"));
        ui->lbPrompt->setText(tr("Add Roster"));
        ui->txtJID->setEnabled(true);
        m_bRequest = false;
    }
    if(groups.isEmpty())
    {
        ui->txtGroup->setCurrentText(tr("My friends"));
    }
    else
    {
        foreach(QString g, groups)
        {
            ui->txtGroup->addItem(g);
        }
    }
    return 0;
}

void CFrmAddRoster::changeEvent(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    }
}

void CFrmAddRoster::on_pbOk_clicked()
{
    QString szJid = ui->txtJID->text();
    if(szJid.isEmpty())
    {
        QMessageBox::critical(this, tr("Error"), tr("Isn't fill JID. Please fill it."));
        return;
    }
    QString szNick = ui->txtNick->text();
    if(szNick.isEmpty())
        szNick = szJid.left(szJid.indexOf("@"));
    QSet<QString> groups;
    groups << ui->txtGroup->currentText();
    CClient::SUBSCRIBE_TYPE type;
    if(m_bRequest)
        type = CClient::SUBSCRIBE_ACCEPT;
    else
        type = CClient::SUBSCRIBE_REQUEST;
    GET_CLIENT->RosterAdd(szJid, type, ui->txtNick->text(), groups);
    this->close();
}

void CFrmAddRoster::on_pbCancel_clicked()
{
    if(m_bRequest)
        GET_CLIENT->RosterAdd(ui->txtJID->text(), CClient::SUBSCRIBE_REFUSE);
    this->close();
}

void CFrmAddRoster::on_txtJID_editingFinished()
{
    LOG_MODEL_DEBUG("Roster", "CFrmAddRoster::on_txtJID_editingFinished");
    if(ui->txtNick->text().isEmpty())
    {
        QString szJid = ui->txtJID->text();
        int nPos = szJid.indexOf("@");
        if(nPos > 0)
            ui->txtNick->setText(szJid.left(nPos));
        else
            ui->txtNick->setText(szJid);
    }
}
