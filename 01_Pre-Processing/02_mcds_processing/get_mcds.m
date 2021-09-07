function [G,F,minComRange] = get_mcds(save_nodes,comRange,increment,use_lp,path_export_lp_problem)
    xPos = save_nodes.xPos';
    yPos = save_nodes.yPos';
    N = length(xPos);
    F = [];
    minComRange = comRange-increment;
    while(isempty(F))
        minComRange = minComRange + increment;
        % Apply unit disk connectivtiy
        connectivity = zeros(N);
        for nodeID = 1:N
            distances = sqrt(((xPos(nodeID) - xPos) .^ 2) + ((yPos(nodeID) - yPos) .^ 2));
            connectivity(nodeID,distances<minComRange) = 1;
        end
        G = graph(connectivity,'upper','omitselfloops');
        G.Nodes.ID = [1:N]';
        G.Nodes.xPos = xPos';
        G.Nodes.yPos = yPos';

        if(use_lp)
            F = optimization_search_MCDS(G,path_export_lp_problem,minComRange);
        else
            F = exhaustive_search_MCDS(G);
        end
    end
    F = subgraph(G,F(:));
end

