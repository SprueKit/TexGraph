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
Modified in 2016 by Jonathan Sandusky to add some extra flexibility through signal emission and support
multiple selections / rubberbanding

Removed serialization (handled by SprueEngine::Graph)
*/

#ifndef QNODESEDITOR_H
#define QNODESEDITOR_H

#include <QObject>
#include <qpainter.h>

class QRubberBand;
class QGraphicsView;
class QGraphicsScene;
class QNEConnection;
class QGraphicsItem;
class QPointF;
class QNEBlock;
class QNEPort;

class QNodesEditor : public QObject
{
	Q_OBJECT
public:
	explicit QNodesEditor(QObject *parent = 0);

    void install(QGraphicsScene *scene, QGraphicsView*);

	bool eventFilter(QObject *, QEvent *);

    /// JSandusky: if a node connection is actively being dragged then terminate it
    void haltConnection();

signals:
    /// Emitted before a node/connection is deleted, set "allowed" to false to deny deletion
    void AllowDelete(bool& allowed, QGraphicsItem*);
    /// Emitted before a connection is formed between two ports, set "allowed" to false to deny
    void AllowConnect(bool& allowed, QNEPort* lhs, QNEPort* rhs);
    /// Emitted whenever a connection is deleted, no denial support
    void ConnectionDeleted(QNEPort* lhs, QNEPort* rhs);
    /// Emitted whever a QNEBlock is deleted, no denial support
    void NodeDeleted(QNEBlock* block);
public:
	QGraphicsItem *itemAt(const QPointF&);
private:
    QPointF startRubber_;
    QRubberBand* rubber_;

private:
    QPainterPath lastSelectionpath_;
	QGraphicsScene *scene_;
    QGraphicsView* view_;
	QNEConnection *conn_;
    /// If the object self forms the connections than connection creation doesn't occur here, it occurs as the result of a callback or other explicit construction of the connection
    bool objectSelfFormsConnections_ = true;
	// QNEBlock *selBlock;
};

#endif // QNODESEDITOR_H
