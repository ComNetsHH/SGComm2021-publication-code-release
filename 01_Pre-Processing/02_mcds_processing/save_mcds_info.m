function [] = save_mcds_info(full_graph, mcds_graph, path_export_feeder_info,ID,comRange)
    mcds_info.full_graph = full_graph;
    mcds_info.mcds_graph = mcds_graph;
    mcds_info.comRange = comRange;
    mcds_info.numNodes = length(full_graph.Nodes.xPos);
    save(strcat(path_export_feeder_info,"/mcds",num2str(ID),".mat"),"mcds_info");
end

