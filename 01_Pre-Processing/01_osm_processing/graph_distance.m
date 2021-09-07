function [distances] = graph_distance(graph,buildings,X,obj)

    % This routine is not very optimized.
    % However, it is the version used in the publication.

    distances = zeros(1,size(obj,1));
    for objID = 1:size(obj,1)
        objS = obj(objID,:);

        buildingX = [buildings.cX];
        buildingY = [buildings.cY];

        % Find the node to which obj belongs
        targetID = find(buildingX==objS(1));
        for candidateID = 1:length(targetID)
            candidate = targetID(candidateID);
            if(buildingY(candidate) == objS(2))
                targetID = candidate;
                break;
            end
        end

        % Find the node to which obj belongs
        sourceID = find(buildingX==X(1));
        for candidateID = 1:length(sourceID)
            candidate = sourceID(candidateID);
            if(buildingY(candidate) == X(2))
                sourceID = candidate;
                break;
            end
        end

        nodes = table2array(graph.Nodes);
        buildingIDs = nodes(nodes(:,3) == 2,:);

        % Return shortest path between both nodes
        [~,d] = shortestpath(graph,buildingIDs(sourceID,4),buildingIDs(targetID,4));
        if(isinf(d))
            disp("alert, please have a look at the distance calculation");
        end
        distances(objID)=d;
    end
end

