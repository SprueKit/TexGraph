#pragma once

#include <EditorLib/Controls/ISignificantControl.h>
#include <EditorLib/Selectron.h>

#include <QWidget>
#include <QCheckbox>
#include <QComboBox>
#include <QImage>
#include <QPixmap>

class QGraphicsItem;
class QGraphicsTextItem;
class QGraphicsView;
class QGraphicsScene;
class QGraphicsPixmapItem;

namespace SprueEditor
{

class TextureInspector : public QWidget, public SelectronLinked, public ISignificantControl
{
    Q_OBJECT;
public:
    TextureInspector(QWidget* parent = 0x0);
    virtual ~TextureInspector();

    virtual void Link(Selectron* sel);

    void showEvent(QShowEvent* event) Q_DECL_OVERRIDE;

    void SetImage(QImage* image);

public slots:
    void ResetView();
    void ImageSizeChanged(int index);
    void SelectionChanged(void* source, Selectron* selectron);
    void SelectionDataChanged(void* source, Selectron* sel, unsigned hash);
    void TileModeChanged(int state);

private:
    QCheckBox* tileMode_;
    QComboBox* sizeCombo_;
    QGraphicsScene* scene_ = 0x0;
    QGraphicsView* view_ = 0x0;
    QGraphicsPixmapItem** textureItems_ = 0x0;
    QGraphicsItem* statusItem_ = 0x0;
    QGraphicsTextItem* statusText_ = 0x0;
};

}