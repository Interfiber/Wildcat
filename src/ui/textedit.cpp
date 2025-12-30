//
// Created by hstasonis on 12/30/25.
//

#include <QPushButton>

#include "Wildcat/ui/textinput.h"

TextInputDialog::TextInputDialog(const std::string &title, QWidget* parent) : QDialog(parent), m_layout(nullptr), m_edit(nullptr), m_ok(nullptr),
                                                    m_cancel(nullptr), m_title(nullptr)
{

    m_layout = new QVBoxLayout;

    m_title = new QLabel();
    m_title->setText(title.data());

    m_edit = new QLineEdit();

    connect(m_edit, &QLineEdit::textChanged, this, [this] ()
    {
        m_result = m_edit->text().toStdString();
    });

    m_ok = new QPushButton("Ok");
    m_cancel = new QPushButton("Cancel");

    connect(m_ok, &QPushButton::clicked, this, [this]
    {
        accept();
    });

    connect(m_cancel, &QPushButton::clicked, this, [this]
    {
        reject();
    });

    m_buttonLayout = new QHBoxLayout;
    m_buttonLayout->addWidget(m_ok);
    m_buttonLayout->addWidget(m_cancel);

    m_layout->addWidget(m_title);
    m_layout->addWidget(m_edit);
    m_layout->addLayout(m_buttonLayout);

    setLayout(m_layout);
}

TextInputDialog::~TextInputDialog()
{
    delete m_title;
    delete m_ok;
    delete m_cancel;

    delete m_buttonLayout;
    delete m_layout;;
}
