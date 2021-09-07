function [H] = plot_mcds_results(full_graph,bb_graph)
    H = plot(full_graph,'XData',full_graph.Nodes.xPos,'YData',full_graph.Nodes.yPos);
    highlight(H,bb_graph.Nodes.ID,'NodeColor','r');
    for bbNodeID = 1:length(bb_graph.Nodes.ID)
        nodeID = bb_graph.Nodes.ID(bbNodeID);
        nodeIDneigh = neighbors(full_graph,nodeID);
        highlight(H,nodeIDneigh,'NodeColor','c');
    end
    highlight(H,bb_graph.Nodes.ID,'NodeColor','k','Marker','S','MarkerSize',8);
end

