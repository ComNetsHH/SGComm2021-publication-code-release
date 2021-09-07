function [isCDS] = check_if_CDS(adjMatrix,nodeSet)
    % check first if set is dominating
    % We need at least one edge from nodeSet to all other nodes
    if(nnz(sum(adjMatrix(nodeSet,:),1)) == length(adjMatrix))
        % check if nodeSet is connected, by checking Fiedler Distance
        % (https://en.wikipedia.org/wiki/Algebraic_connectivity)
        nodeSetAdjMatrix = adjMatrix(nodeSet,nodeSet);
        degreeMatrix = sum(nodeSetAdjMatrix);
        laplacian = degreeMatrix.*speye(length(nodeSet))-nodeSetAdjMatrix;
        eigValues = sort(eig(laplacian));
        
        % Check for numerical zero
        if(eigValues(2)>0.0001)
            % Is a CDS
            isCDS = true;
            return
        end
    % Special case for only one node, it is always connected
    elseif(length(nodeSet)==1)
        isCDS = true;
        return
    end
    % Not a CDS
    isCDS = false;
    return
end

