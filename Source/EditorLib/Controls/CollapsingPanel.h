#pragma once

#include <QGroupBox>

/// QGroupBox that is designed to collapse the contents when 'checked'
/// Assumes existince of a style switched on the boolean property 'collapser'
/// Style (in QDarkStyle) changes the checkbox into an arrow indicator.
class CollapsingPanel : public QGroupBox
{
    Q_OBJECT;
public:
    CollapsingPanel(const QString& title);
    virtual ~CollapsingPanel() { }

    void SetDirty(bool state);
private:
    QString titleText_;
};