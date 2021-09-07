function [H] = plot_osm_results(graph,feederMapping)

    idx = feederMapping.idx;
    C = feederMapping.C;

    figure;
    H = plot(graph,'XData',graph.Nodes.xPos,'YData',-graph.Nodes.yPos);

    nodes = table2array(graph.Nodes);
    buildingIDs = nodes(nodes(:,3) == 2,4);
    streetIDs = nodes(nodes(:,3) == 1,4);

    highlight(H,buildingIDs,'NodeColor','r');
    highlight(H,buildingIDs,'MarkerSize',3);
    highlight(H,streetIDs,'MarkerSize',0.001);

    colors = hsv(length(C));
    for buildingID = 1:length(buildingIDs)
        highlight(H,buildingIDs(buildingID),'NodeColor',colors(idx(buildingID),:));
    end

end

