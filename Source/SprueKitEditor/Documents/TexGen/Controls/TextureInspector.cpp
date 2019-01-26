#include "TextureInspector.h"

#include "../../GlobalAccess.h"
#include "../../Data/SprueDataSources.h"

#include "../../Documents/TexGen/Tasks/TextureInspectorGenTask.h"

#include <QApplication>
#include <QBoxLayout>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLabel>
#include <QPushButton>
#include <QScrollbar>
#include <qevent.h>

#include <SprueEngine/TextureGen/TextureNode.h>

namespace SprueEditor
{

    static const int TextureInspectorImageSizes[] = {
        128,
        256,
        512,
        1024,
        2048,
        4096
    };

    class TextureInspectorGraphicsView : public QGraphicsView
    {
    public:
        TextureInspectorGraphicsView(TextureInspector* inspector) :
            inspector_(inspector)
        {
            setMouseTracking(true);
        }

        virtual void mouseMoveEvent(QMouseEvent* evt)
        {
            // Support middle mouse drag scrolling
            if (evt->buttons() & Qt::MouseButton::MiddleButton || evt->buttons() && Qt::MouseButton::LeftButton)
            {
                QPoint delta = evt->pos() - lastMouse_;
                int vPos = verticalScrollBar()->value();
                int hPos = horizontalScrollBar()->value();
                verticalScrollBar()->setValue(vPos - delta.y() * 2);
                horizontalScrollBar()->setValue(hPos - delta.x() * 2);
                evt->accept();
                lastMouse_ = evt->pos();
                return;
            }
            lastMouse_ = evt->pos();

            QGraphicsView::mouseMoveEvent(evt);
        }

        virtual void wheelEvent(QWheelEvent* evt)
        {
            if (QApplication::keyboardModifiers() & Qt::KeyboardModifier::ControlModifier)
            {
                SetZoom(currentZoom_ + (evt->delta() > 0 ? 1 : -1));
                return;
            }
            QGraphicsView::wheelEvent(evt);
        }

        void SetZoom(int level)
        {
            const int GraphZoomLevelsCt = 16;

            currentZoom_ = level;
            currentZoom_ = std::min(std::max(currentZoom_, 0), GraphZoomLevelsCt - 1);

            const float GraphZoomLevels[] = {
                0.25f,
                0.5f,
                0.75f,
                1.0f,
                1.25f,
                1.5f,
                1.75f,
                2.0f,
                2.25f,
                2.5f,
                2.75f,
                3.0f,
                3.5f,
                4.0f,
                4.5f,
                5.0f,
            };

            QTransform trans;
            trans.scale(GraphZoomLevels[currentZoom_], GraphZoomLevels[currentZoom_]);
            setTransform(trans, false);
        }

        TextureInspector* inspector_;
        QPoint lastMouse_;
        int currentZoom_ = 3;
    };

    TextureInspector::TextureInspector(QWidget* parent) :
        QWidget(parent)
    {
        QWidget* barWidget = new QWidget(this);
        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setMargin(2);
        layout->addWidget(barWidget);
        
        QHBoxLayout* barLayout = new QHBoxLayout(barWidget);
        barLayout->setAlignment(Qt::AlignLeft);
        barLayout->setMargin(2);

        barLayout->addWidget(new QLabel("Size"));
        barLayout->addWidget(sizeCombo_ = new QComboBox());

        sizeCombo_->addItem("128x128", 128);
        sizeCombo_->addItem("256x256", 256);
        sizeCombo_->addItem("512x512", 512);
        sizeCombo_->addItem("1024x1024", 1024);
        sizeCombo_->addItem("2048x2048", 2048);
        sizeCombo_->addItem("4096x4096", 4096);

        scene_ = new QGraphicsScene();
        view_ = new TextureInspectorGraphicsView(this);
        view_->setScene(scene_);
        layout->addWidget(view_);
        textureItems_ = new QGraphicsPixmapItem*[4];
        textureItems_[0] = new QGraphicsPixmapItem();
        textureItems_[1] = new QGraphicsPixmapItem();
        textureItems_[2] = new QGraphicsPixmapItem();
        textureItems_[3] = new QGraphicsPixmapItem();
        scene_->addItem(textureItems_[0]);
        scene_->addItem(textureItems_[1]);
        scene_->addItem(textureItems_[2]);
        scene_->addItem(textureItems_[3]);
        sizeCombo_->setCurrentIndex(1);
        connect(sizeCombo_, static_cast<void(QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &TextureInspector::ImageSizeChanged);

        barLayout->addWidget(tileMode_ = new QCheckBox("Tile"));
        connect(tileMode_, &QCheckBox::stateChanged, this, &TextureInspector::TileModeChanged);

        QPushButton* resetView = new QPushButton("Reset View");
        barLayout->addWidget(resetView, 0, Qt::AlignRight);
        connect(resetView, &QPushButton::clicked, this, &TextureInspector::ResetView);

        auto rect = new QGraphicsPathItem();
        QPainterPath path;
        path.addRoundedRect(0, 0, 200, 40, 10, 10);
        rect->setPath(path);
        rect->setBrush(QColor::fromRgb(80, 80, 80));
        statusText_ = new QGraphicsTextItem("Generating preview...", rect);
        statusText_->setDefaultTextColor(QColor::fromRgb(230, 230, 230));
        statusText_->setPos(rect->pos().x() + 5, rect->pos().y() + 7);
        statusItem_ = rect;
        statusItem_->setVisible(false);
        scene_->addItem(rect);
    }

    TextureInspector::~TextureInspector()
    {

    }

    void TextureInspector::showEvent(QShowEvent* event)
    {
        SelectionChanged(0x0, GetSelectron());
    }

    void TextureInspector::SetImage(QImage* image)
    {
        if (image)
        {
            statusItem_->setVisible(false);
            QPixmap& pix = QPixmap::fromImage(*image);
            textureItems_[0]->setPixmap(pix);
            textureItems_[1]->setX(textureItems_[0]->x() + textureItems_[0]->boundingRect().width());

            textureItems_[2]->setY(textureItems_[0]->y() + textureItems_[0]->boundingRect().height());

            textureItems_[3]->setX(textureItems_[0]->x() + textureItems_[0]->boundingRect().width());
            textureItems_[3]->setY(textureItems_[0]->y() + textureItems_[0]->boundingRect().height());
            if (tileMode_->isChecked())
            {
                textureItems_[1]->setPixmap(QPixmap::fromImage(*image));
                textureItems_[2]->setPixmap(QPixmap::fromImage(*image));
                textureItems_[3]->setPixmap(QPixmap::fromImage(*image));
            }
            else
            {
                textureItems_[1]->setPixmap(QPixmap());
                textureItems_[2]->setPixmap(QPixmap());
                textureItems_[3]->setPixmap(QPixmap());
            }
        }
    }

    void TextureInspector::ResetView()
    {
        ((TextureInspectorGraphicsView*)view_)->SetZoom(3);
        view_->centerOn(textureItems_[0]);
    }

    void TextureInspector::ImageSizeChanged(int index)
    {
        SelectionChanged(0x0, GetSelectron());
    }

    void TextureInspector::Link(Selectron* sel)
    {
        connect(sel, &Selectron::SelectionChanged, this, &TextureInspector::SelectionChanged);
        connect(sel, &Selectron::DataChanged, this, &TextureInspector::SelectionDataChanged);
    }

    void TextureInspector::SelectionChanged(void* src, Selectron* sel)
    {
        if (!isVisible())
            return;
        if (auto graphNode = sel->GetMostRecentSelected<GraphNodeDataSource>())
        {
            if (auto textureNode = dynamic_cast<SprueEngine::TextureNode*>(graphNode->GetNode()))
            {
                if (textureNode->CanPreview())
                {
                    if (auto taskMan = Global_SecondaryTaskProcessor())
                    {
                        statusText_->setPlainText("Generating preview...");
                        statusItem_->setVisible(true);
                        std::shared_ptr<Task> task = std::make_shared<TextureInspectorGenTask>(textureNode->graph, textureNode, TextureInspectorImageSizes[sizeCombo_->currentIndex()], TextureInspectorImageSizes[sizeCombo_->currentIndex()]);
                        taskMan->AddTask(task);
                    }
                }
                else
                {
                    statusText_->setPlainText("Nothing to preview");
                    statusItem_->setVisible(true);
                    textureItems_[0]->setPixmap(QPixmap());
                    textureItems_[1]->setPixmap(QPixmap());
                    textureItems_[2]->setPixmap(QPixmap());
                    textureItems_[3]->setPixmap(QPixmap());
                }
            }
        }
        else
        {
            statusText_->setPlainText("Nothing to preview");
            statusItem_->setVisible(true);
            textureItems_[0]->setPixmap(QPixmap());
            textureItems_[1]->setPixmap(QPixmap());
            textureItems_[2]->setPixmap(QPixmap());
            textureItems_[3]->setPixmap(QPixmap());
        }
        ResetView();
    }

    void TextureInspector::SelectionDataChanged(void* source, Selectron* sel, unsigned hash)
    {
        SelectionChanged(source, sel);
    }

    void TextureInspector::TileModeChanged(int state)
    {
        if (tileMode_->isChecked())
        {
            textureItems_[1]->setPixmap(textureItems_[0]->pixmap());
            textureItems_[2]->setPixmap(textureItems_[0]->pixmap());
            textureItems_[3]->setPixmap(textureItems_[0]->pixmap());
        }
        else
        {
            textureItems_[1]->setPixmap(QPixmap());
            textureItems_[2]->setPixmap(QPixmap());
            textureItems_[3]->setPixmap(QPixmap());
        }
    }
}