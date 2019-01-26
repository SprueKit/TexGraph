#include "RibbonPage.h"

#include "RibbonSection.h"

#include <QFrame>

RibbonPage::RibbonPage(const QString& pageTitle) :
    QWidget(),
    title_(pageTitle)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    sectionsLayout_ = new QHBoxLayout(this);
    sectionsLayout_->setAlignment(Qt::AlignLeft);
    sectionsLayout_->setMargin(0);
    sectionsLayout_->setSpacing(0);
    sectionsLayout_->addItem(new QSpacerItem(1, 1, QSizePolicy::Expanding));
}

RibbonPage::~RibbonPage()
{
    for (auto section : sections_)
        delete section;
    delete sectionsLayout_;
}

void RibbonPage::AddSection(RibbonSection* section)
{
    if (sections_.size() > 0)
    {
        QFrame* line = new QFrame();
        line->setFixedWidth(1);
        line->setFrameShape(QFrame::VLine);
        line->setFrameShadow(QFrame::Plain);
        sectionsLayout_->insertWidget(sectionsLayout_->count() - 1, line, 0, Qt::AlignLeft);
        //sectionsLayout_->addWidget(line);
    }
    sections_.push_back(section);
    sectionsLayout_->insertWidget(sectionsLayout_->count() - 1, section, 0, Qt::AlignLeft);
    //sectionsLayout_->addWidget(section);
}

RibbonSection* RibbonPage::GetSection(unsigned index)
{
    if (index < sections_.size())
        return sections_[index];
    return 0x0;
}

RibbonSection* RibbonPage::RemoveSection(unsigned index)
{
    if (index < sections_.size())
    {
        auto ret = sections_[index];
        sectionsLayout_->removeWidget(sections_[index]);
        sections_.erase(sections_.begin() + index);
        return ret;
    }
    return 0x0;
}

RibbonSection* RibbonPage::RemoveSection(RibbonSection* section)
{
    auto found = std::find(sections_.begin(), sections_.end(), section);
    if (found != sections_.end())
    {
        sectionsLayout_->removeWidget(*found);
        sections_.erase(found);
        return section;
    }
    return 0x0;
}

void RibbonPage::SetPageTitle(const QString& title)
{
    title_ = title;
    //TODO send event
}

void RibbonPage::SetActive(bool state)
{
    active_ = state;
    emit ActiveChanged(this);
}