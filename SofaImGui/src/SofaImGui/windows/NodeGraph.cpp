/******************************************************************************
 *                 SOFA, Simulation Open-Framework Architecture                *
 *                    (c) 2006 INRIA, USTL, UJF, CNRS, MGH                     *
 *                                                                             *
 * This program is free software; you can redistribute it and/or modify it     *
 * under the terms of the GNU General Public License as published by the Free  *
 * Software Foundation; either version 2 of the License, or (at your option)   *
 * any later version.                                                          *
 *                                                                             *
 * This program is distributed in the hope that it will be useful, but WITHOUT *
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for    *
 * more details.                                                               *
 *                                                                             *
 * You should have received a copy of the GNU General Public License along     *
 * with this program. If not, see <http://www.gnu.org/licenses/>.              *
 *******************************************************************************
 * Authors: The SOFA Team and external contributors (see Authors.txt)          *
 *                                                                             *
 * Contact information: contact@sofa-framework.org                             *
 ******************************************************************************/
#include <SofaImGui/windows/NodeGraph.h>
#include <imgui.h>
#include <ImNodeFlow.h>

namespace windows {

struct NodeNode : ImFlow::BaseNode
{
    explicit NodeNode(sofa::simulation::Node::SPtr node)
    {
        this->setTitle(node->getName());
    }
};

struct ComponentNode : ImFlow::BaseNode
{
    explicit ComponentNode(sofa::core::objectmodel::BaseObject::SPtr component)
    {
        this->setTitle(component->getName());
    }
};

void addNode(ImFlow::ImNodeFlow& editor, sofa::simulation::Node::SPtr node, std::size_t depth)
{
    editor.addNode<NodeNode>(ImVec2{depth * 150.f, 0.f}, node);

    ++depth;
    for (auto& child : node->getChildren())
    {
        if (auto* childNode = dynamic_cast<sofa::simulation::Node*>(child))
        {
            addNode(editor, childNode, depth);
        }
    }

    float height = 0;
    for (auto& child : node->getNodeObjects())
    {
        editor.addNode<ComponentNode>(ImVec2{depth * 150.f, height += 50.f}, child);
    }
}

void addRootNode(ImFlow::ImNodeFlow& editor, sofa::simulation::Node::SPtr node)
{
    std::size_t depth = 0;
    addNode(editor, node, depth);
}

void showNodeGraph(sofa::core::sptr<sofa::simulation::Node> groot, const char* const& windowNameNodeGraph, WindowState& winManagerNodeGraph)
{
    if (*winManagerNodeGraph.getStatePtr())
    {
        if (ImGui::Begin(windowNameNodeGraph, winManagerNodeGraph.getStatePtr()))
        {
            static std::map<sofa::simulation::Node*, ImFlow::ImNodeFlow> nodeEditor;
            auto nodeIt = nodeEditor.find(groot.get());
            if (nodeIt == nodeEditor.end())
            {
                const auto res = nodeEditor.insert(std::make_pair(groot.get(), ImFlow::ImNodeFlow()));
                if (res.second) {
                    nodeIt = res.first;
                }
                addRootNode(nodeIt->second, groot);
            }
            nodeIt->second.update();
        }
        ImGui::End();

    }
}
} // namespace windows
