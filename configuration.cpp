#include "configuration.h"
#include "ui_configuration.h"

#include <QSerialPortInfo>

#include "connection.h"

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
    ui->setupUi(this);
    this->setWindowTitle("Control de accesos - Configuración");

    connection conn;
    QSqlQuery* qry=new QSqlQuery(conn.mydb);

    QSqlQueryModel * companyModal=new QSqlQueryModel();
    qry->prepare("select name from company order by name asc");
    qry->exec();
    companyModal->setQuery(*qry);
    ui->comboBox->setModel(companyModal);

    qry->prepare("select co.key, co.location, em.name from configuration as co inner join company as em on co.rut_company=em.rut where co.key=(select key from users where rut = '"+rutSignin+"')");
    qry->exec();
    while(qry->next()){
        ui->lineEdit_key->setText(qry->value(0).toString());
        ui->lineEdit_place->setText(qry->value(1).toString());
        int index = ui->comboBox->findText(qry->value(2).toString(),Qt::MatchCaseSensitive);

        if (index != -1) // -1 for not found
            ui->comboBox->setCurrentIndex(index);
        else
            delete companyModal;
    }
    qry->clear();

    QSqlQueryModel * TableModal=new QSqlQueryModel();
    qry->prepare("select name as Nombre,serial as Serial,baudrate as Baudrate from devices where key=(select key from users where rut = '"+rutSignin+"')");
    qry->exec();
    TableModal->setQuery(*qry);
    ui->tableView->setModel(TableModal);

    if(conn.getFirstFromDb(rutSignin, "select take_picture from configuration where key=(select key from users where rut = '"+rutSignin+"')") == "YES"){
        ui->radioButton_picture_yes->setChecked(true);
    }else{
        ui->radioButton_picture_no->setChecked(true);
    }

    if(conn.getFirstFromDb(rutSignin, "select output_register from configuration where key=(select key from users where rut = '"+rutSignin+"')") == "YES"){
        ui->radioButton_output_yes->setChecked(true);
    }else{
        ui->radioButton_output_no->setChecked(true);
    }

    delete qry;

    // RTScan
    foreach (const QSerialPortInfo &Ports, QSerialPortInfo::availablePorts())
        ui->comboBox_rtscan->addItem(Ports.portName());
    rtPort = ui->comboBox_rtscan->currentText();
}

Configuration::~Configuration()
{
    delete ui;
}

void Configuration::on_pushButton_close_clicked()
{
    this->close();
}

void Configuration::on_pushButton_save_clicked()
{

    connection conn;
    QSqlQuery* qry=new QSqlQuery(conn.mydb);
    qry->prepare("UPDATE configuration SET location = '"+ui->lineEdit_place->text()+
                 "', rut_company = '"+ui->comboBox->currentIndex()+
                 "', rtscan_port = '"+ui->comboBox_rtscan->currentText()+
                 "' where key='"+ui->lineEdit_key->text()+"'");
    if(!qry->exec())
    {
        QMessageBox::critical(this,tr("Error al actualizar"),error1);
        Logger::insert2Logger(rutSignin, " ERROR ", qry->lastError().text()+" -> "+qry->executedQuery());
    }
    else
    {
        QMessageBox::information(this,"ACTUALIZACION", tr("Configuración actualizada exitosamente."));
        Logger::insert2Logger(rutSignin, " DEBUG ", qry->lastQuery());
    }
    delete qry;

    if(ui->radioButton_picture_yes->isChecked()){
        //save to db
    }else{
        //save to db
    }

    if(ui->radioButton_output_yes->isChecked()){
        //save to db
    }else{
        //save to db
    }
}
