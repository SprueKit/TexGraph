/* Copyright (c) 2012, STANISLAW ADASZEWSKI
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of STANISLAW ADASZEWSKI nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL STANISLAW ADASZEWSKI BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

/* 
Modified in 2016 by Jonathan Sandusky to add some delegation capability for nodes and ports
that are more flexible in visual presentation and color theming
*/

#ifndef QNEBLOCK_H
#define QNEBLOCK_H

#include <QGraphicsPathItem>
#include <QImage>
#include <QIcon>

#include <memory>

class QNEPort;
class QNEBlock;

typedef void(*BLOCK_MOVED_CALLBACK)(QNEBlock* block);
typedef void (*BRUSH_STRATEGY)(QNEPort* port);
typedef void (*PREVIEW_TILE_PROVIDER)(QNEBlock*);

/// Delegate type for specialized behaviors of the Block
struct NodeBlockModel
{
    /// Setup the graphics child item for the port (ie. type indication), used when ports are added
    virtual void ConstructBrush(QNEPort* port) = 0;
    /// Determine if the block requires a preview image or not, used during block construction
    virtual bool RequiresPreview(QNEBlock* block, QSize& previewSize) = 0;
    /// Grab the latest available preview image, used during paint
    virtual void UpdateTilePreview(QNEBlock* block) = 0;
};

class QNEBlock : public QGraphicsPathItem
{
public:
	enum { Type = QGraphicsItem::UserType + 3 };

    QNEBlock(BLOCK_MOVED_CALLBACK = 0, QGraphicsItem *parent = 0);
    ~QNEBlock();

	QNEPort* addPort(const QString &name, bool isOutput, int flags = 0, void* ptr = 0, BRUSH_STRATEGY strat = 0);
	void addInputPort(const QString &name);
	void addOutputPort(const QString &name);
	void addInputPorts(const QStringList &names);
	void addOutputPorts(const QStringList &names);
    void clearOutputPorts();
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
	QNEBlock* clone();
	QVector<QNEPort*> ports();

    QNEPort* findPort(void* ptr);

	int type() const { return Type; }

    void setUserData(void* value) { userData = value; }
    void* getUserData() const { return userData; }

    void setTitleColor(const QColor& color) { titleColor_ = color; }
    void setBackgroundColor(const QColor& color) { backgroundColor_ = color; }
    void setLabelColor(const QColor& color) { labelColor_ = color; }
    void setPreviewProvider(PREVIEW_TILE_PROVIDER prov) { previewProvider_ = prov; }
    void SetPreviewImage(std::shared_ptr<QImage> img) { previewImage_ = img; calculateLayout(); }
    void updatePreview();

    void setIcon(QIcon icon) { icon_ = icon; calculateLayout(); }

    /// Helper method for finding a QNEBlock in a scene based on the user data.
    static QNEBlock* FindBlock(QGraphicsScene* scene, void* userData);

protected:
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);
    void calculateLayout();

private:
    PREVIEW_TILE_PROVIDER previewProvider_;
    BLOCK_MOVED_CALLBACK moveCallback_ = 0x0;
    QIcon icon_;
    QColor titleColor_;
    QColor backgroundColor_;
    QColor labelColor_;
    std::shared_ptr<QImage> previewImage_;
    void* userData;
	int horzMargin;
	int vertMargin;
	int width;
	int height;
};

#endif // QNEBLOCK_H
