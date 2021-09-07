function [] = save_feeder_info(graph,feederMapping,path_export_feeder_info)
    %%
    idx = feederMapping.idx;
    nodes = table2array(graph.Nodes);
    buildingIDs = nodes(nodes(:,3) == 2,4);
    
    % For each feeder
    for cID = 1:max(idx)
        % Extract all node info for said feeder
        extracted_nodes = graph.Nodes(buildingIDs(idx==cID),:);
        save_nodes = [];
        save_nodes.xPos = 1000.*extracted_nodes.xPos;
        save_nodes.yPos = 1000.*extracted_nodes.yPos;
        disp(strcat("Saving Feeder ", num2str(cID), " Containing ", num2str(length(save_nodes.yPos)), " Nodes..."));
        save(strcat(path_export_feeder_info,"/feeder",num2str(cID),".mat"),"save_nodes");
    end
end

