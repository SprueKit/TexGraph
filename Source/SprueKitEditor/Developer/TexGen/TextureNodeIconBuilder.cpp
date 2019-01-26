#include "TextureNodeIconBuilder.h"

#include "../../Data/TexGenData.h"

#include <SprueEngine/Core/Context.h>
#include <SprueEngine/TextureGen/TextureNode.h>
#include <SprueEngine/TextureGen/TexGenImpl.h>
#include <SprueEngine/Loaders/BasicImageLoader.h>

#include <QString>

using namespace SprueEngine;

namespace SprueEditor
{

    TextureNodeIconBuilder::TextureNodeIconBuilder()
    {

        static const std::string FilterFiles[] = {
            "C:/TextureNodeImages/Bases/Mandrill.png",
            "C:/TextureNodeImages/Bases/ColorGrid.png",
            "C:/TextureNodeImages/Bases/YinYang.png",
        };
        static const std::string FilterNames[] = {
            "Mandrill",
            "ColorGrid",
            "YinYang"
        };
        static const int fileCount = 3;

        auto allNodeNames = TexGenData::NodeNames;
        for (auto record : allNodeNames)
        {
            auto nodeHash = record.first;

            for (unsigned i = 0; i < fileCount; ++i)
            {
                Graph* graph = new Graph();
                if (auto textureNode = SprueEngine::Context::GetInstance()->Create<SprueEngine::TextureNode>(nodeHash))
                {
                    textureNode->Construct();
                    graph->AddNode(textureNode, true);

                    BitmapGenerator* bitMapGen = 0x0;
                    QString fileName = QString("C:/TextureNodeImages/%1.png").arg(textureNode->GetTypeName());
                    if (textureNode->inputSockets.size())
                    {
                        bitMapGen = new BitmapGenerator();
                        bitMapGen->Construct();
                        bitMapGen->SetProperty(StringHash("Image"), ResourceHandle("Image", FilterFiles[i]));
                        fileName = QString("C:/TextureNodeImages/%1_%2.png").arg(textureNode->GetTypeName()).arg(FilterNames[i].c_str());

                        graph->AddNode(bitMapGen, false);
                        for (auto socket : textureNode->inputSockets)
                            graph->Connect(bitMapGen->GetOutputSocket(0), socket);
                    }
                
                    if (auto image = textureNode->GetPreview(64, 64))
                        BasicImageLoader::SavePNG(image.get(), fileName.toStdString().c_str());
                }
                delete graph;
            }
        }

    }

}