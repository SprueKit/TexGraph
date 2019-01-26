#pragma once

#include <QApplication>
#include <QClipboard>
#include <qlayout.h>
#include <qmimedata.h>
#include <qfontmetrics.h>
#include <qmenu.h>
#include <qmenubar.h>
#include <QAbstractButton>

#include <vector>

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/IEditable.h>
#include <SprueEngine/VectorBuffer.h>

class QPainter;
class QFontMetrics;

namespace SprueEngine
{
    class IEditable;
}

namespace SprueEditor
{
    namespace QtHelpers
    {
        /// Removes and destroys everything contained by a layout.
        void ClearLayout(QLayout* layout);
        /// Hides everything contained by a layout.
        void HideLayout(QLayout* layout);
        /// Shows everything contained by a layout.
        void ShowLayout(QLayout* layout);
        /// Sets the enable state of all contents of the given layout.
        void SetEnabledOnAll(QLayout* layout, bool state);

        void UpdateMenus(QMenuBar* bar);
        void UpdateMenus(QMenu* bar);

        template<class T>
        void CopyToClipboard(const std::vector<T*>& editables, const char* mime, void (*POST_CALL)(const std::vector<T*>&, SprueEngine::VectorBuffer&) = 0x0)
        {
            if (editables.size() > 0)
            {
                QClipboard* clip = QApplication::clipboard();
                SprueEngine::VectorBuffer buffer;
                buffer.WriteUInt(editables.size());
                for (auto editable : editables)
                    editable->Serialize(&buffer);
                if (POST_CALL)
                    POST_CALL(editables, buffer);

                QMimeData* data = new QMimeData();
                QByteArray array(buffer.GetSize(), Qt::Initialization::Uninitialized);
                memcpy(array.data(), buffer.GetData(), buffer.GetSize());
                data->setData(mime, array);
                clip->setMimeData(data);
            }
        }

        template<class T>
        void RetrieveFromClipboard(std::vector<T*>& editables, const char* mime, void(*POST_CALL)(std::vector<T*>&, SprueEngine::VectorBuffer&) = 0x0)
        {
            QClipboard* clip = QApplication::clipboard();
            if (const QMimeData* data = clip->mimeData())
            {
                QByteArray array = data->data(mime);
                if (array.size() > 0)
                {
                    SprueEngine::VectorBuffer buff(array.data(), array.size());
                    unsigned ct = buff.ReadUInt();
                    while (ct)
                    {
                        if (T* deserialized = Context::GetInstance()->Deserialize<T>(&buff))
                            editables.push_back(deserialized);
                        --ct;
                    }

                    if (POST_CALL)
                        POST_CALL(editables, buff);
                }
            }
        }

        /// Get the path to something in the AppData/Roaming directory
        std::string GetAppDataPath(const std::string& file);
        /// Get the path to something in the Users/USERNAME/Documents directory
        std::string GetDocumentsPath(const std::string& file);

        QSize MeasureTextForceWrap(const QString& text, const QFontMetrics& metrics, int width, std::vector<std::pair<int, int> >* indices);
        void PaintTextForceWrap(QPainter* painter, const QPoint& paintAt, const QString& text, QFontMetrics& metrics, int width);

        void AttachHelpFlyout(QAbstractButton* button, const QString& name);
    }
}