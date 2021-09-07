function [F] = exhaustive_search_MCDS(G)
% Exhaustive Search for MCDS F of G
    nodes = G.Nodes.ID;
    numNodes = numnodes(G);

    %% Check wether G is connected
    if length(unique(conncomp(G)))>1
        F = [];
        %disp("### Error: Graph G is not connected and no MCDS can be calculated! Increasing communication range...");
        return;
    end

    %% Generate node degrees
    nodeDegrees = degree(G);
    [nodeDegrees,idx] = sort(nodeDegrees,'descend');
    nodes = nodes(idx);

    %% Create adjacency matrix
    adjMatrix = adjacency(G);

    %% Find starting cardinality
    startCardinality = 1;
    % We need to connect to each node at least once, so we can skip
    % cardinalities for which even the highest degree nodes do not suffice
    while(sum(nodeDegrees(1:startCardinality)) < numNodes-1)
        startCardinality = startCardinality + 1;
    end

        %% For each cardinality 
        for cardinality = startCardinality:numNodes
            % Create all subsets
            disp(strcat("Checking cardinality: ", num2str(cardinality)));
            % This calucations takes a long time and a lot of memory
            subsets = nchoosek(nodes,cardinality);
            for perm = 1:size(subsets,1)
                % Simple check if degree sum is sufficient, abort early
                if(sum(nodeDegrees(subsets(perm,:))) < numNodes-1)
                    continue;
                else
                % Check if CDS
                    if(check_if_CDS(adjMatrix,subsets(perm,:)))
                        F = subsets(perm,:);
                        return
                    end
                end
            end
        end
end

