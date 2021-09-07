function [F] = optimization_search_MCDS(G,exportPath,comRange)
    % Exhaustive Search for MCDS F of G

    nodes = G.Nodes.ID;
    N = numnodes(G);

    %% Check wether G is connected
    if length(unique(conncomp(G)))>1
        F = [];
        %disp("### Error: Graph G is not connected and no MCDS can be calculated! Increasing communication range...");
        return;
    end

    disp("### Info: LP solution approach was selected ...");
    disp("### Info: Problem will not be solved but only processed ...");
    disp("### Info: Please run b2_mcds_processing afterwards ...");

    %% Create adjacency matrix
    mcdsProblem = optimproblem;

    adjMatrix = adjacency(G);
    A = speye(N) + adjMatrix;
    V = triu(adjMatrix);

    % If X(i)==1 vertex i is in the MCDS, [N] 
    X = optimvar('X',N,'Type','integer','LowerBound',0,'UpperBound',1);
    % If Y(i,j)==1 edge (i,j) is required in making the MCDS connected, [NxN]
    Y = optimvar('Y',[N,N],'Type','integer','LowerBound',0,'UpperBound',1);
    % If Z(i,j,k)==1, vertex k is on the side of j of edge(i,j) in the MCDS,
    % [NxNxN]
    Z = optimvar('Z',[N,N,N],'Type','integer','LowerBound',0,'UpperBound',1);

    % Big number for constraint relaxation 
    M = 10000000000000;

    % Minimize number of nodes inside the dominating set
    mcdsProblem.Objective = sum(X);
    % MCDS has to be dominating
    mcdsProblem.Constraints.zero = A'*X >= 1;%

    % Martin connectivity constraints
    n = N; % Control pre-alloc share
    a = repelem([optim.problemdef.OptimizationExpression],n,n);
    mcdsProblem.Constraints.b1 = repelem([optim.problemdef.OptimizationInequality],n,n);
    mcdsProblem.Constraints.b2 = repelem([optim.problemdef.OptimizationInequality],n,n);
    mcdsProblem.Constraints.c1 = repelem([optim.problemdef.OptimizationInequality],n,n,n);
    mcdsProblem.Constraints.c2 = repelem([optim.problemdef.OptimizationInequality],n,n,n);
    mcdsProblem.Constraints.d1 = repelem([optim.problemdef.OptimizationInequality],n,n,n);
    mcdsProblem.Constraints.d2 = repelem([optim.problemdef.OptimizationInequality],n,n,n);
    mcdsProblem.Constraints.e1 = repelem([optim.problemdef.OptimizationInequality],n,n,n);
    mcdsProblem.Constraints.e2 = repelem([optim.problemdef.OptimizationInequality],n,n,n);
    mcdsProblem.Constraints.f1 = repelem([optim.problemdef.OptimizationInequality],n,n);
    mcdsProblem.Constraints.f2 = repelem([optim.problemdef.OptimizationInequality],n,n);
    mcdsProblem.Constraints.y0 = repelem([optim.problemdef.OptimizationEquality],n,n);
    mcdsProblem.Constraints.z0 = repelem([optim.problemdef.OptimizationEquality],n,n,n);

    tic;
    disp(strcat("### Info: Creating optimization problem: ..."));
    for i = 1:N-1
        time = toc;
        disp(strcat("### Info: Progress: ", num2str(100*(i/N))," % in ", num2str(time), " seconds ..."));
        tic
        for j = i+1:N 
            % Handle edges
            if(A(i,j))% element E           
                a(i,j) = Y(i,j);

                mcdsProblem.Constraints.b1(i,j) = Y(i,j) <= X(i);%
                mcdsProblem.Constraints.b2(i,j) = Y(i,j) <= X(j);%

                mcdsProblem.Constraints.c1(i,j,1:N) = Z(i,j,1:N) <= Y(i,j);
                mcdsProblem.Constraints.c2(i,j,1:N) = squeeze(Z(i,j,1:N)) <= X(1:N);

                mcdsProblem.Constraints.d1(j,i,1:N) = Z(j,i,1:N) <= Y(i,j);
                mcdsProblem.Constraints.d2(j,i,1:N) = squeeze(Z(j,i,1:N)) <= X(1:N);
            end

            % (i,j)
            % Handle non-edge vertices
            if(~A(i,j))
                % not element E
                mcdsProblem.Constraints.y0(i,j) = Y(i,j) == 0;
                mcdsProblem.Constraints.z0(i,j,1:N) = Z(i,j,1:N) == 0;      
            end
            % Handle conenctivtiy
            mcdsProblem.Constraints.e1(i,j,1:N) = Y(i,j) - M*(3-X(i)-X(j)-X(1:N)) <= squeeze(Z(i,j,1:N)) + squeeze(Z(j,i,1:N));
            mcdsProblem.Constraints.e2(i,j,1:N) = squeeze(Z(i,j,1:N)) + squeeze(Z(j,i,1:N)) <= Y(i,j) + M*(3-X(i)-X(j)-X(1:N));

            if( i ~= j)
                mcdsProblem.Constraints.f1(i,j) = 1 - M*(2 - X(i) - X(j)) <= sum(Z(i,:,j)) - Z(i,j,j) - Z(i,i,j) + Y(i,j);
                mcdsProblem.Constraints.f2(i,j) = sum(Z(i,:,j)) - Z(i,j,j) - Z(i,i,j) + Y(i,j)  <= 1 + M*(2-X(i)-X(j));
            else
                mcdsProblem.Constraints.f1(i,j) = 1 - M*(2 - X(i) - X(j)) <= sum(Z(i,:,j)) - Z(i,i,j) + Y(i,j);
                mcdsProblem.Constraints.f2(i,j) = sum(Z(i,:,j)) - Z(i,i,j) + Y(i,j)  <= 1 + M*(2-X(i)-X(j));
            end

            % (j,i)
            % Handle non-edge vertices
            if(~A(j,i))
                % not element E
                mcdsProblem.Constraints.y0(j,i) = Y(j,i) == 0;
                mcdsProblem.Constraints.z0(j,i,1:N) = Z(j,i,1:N) == 0;      
            end
            % Handle conenctivtiy
            mcdsProblem.Constraints.e1(j,i,1:N) = Y(j,i) - M*(3-X(j)-X(i)-X(1:N)) <= squeeze(Z(j,i,1:N)) + squeeze(Z(i,j,1:N));
            mcdsProblem.Constraints.e2(j,i,1:N) = squeeze(Z(j,i,1:N)) + squeeze(Z(i,j,1:N)) <= Y(j,i) + M*(3-X(j)-X(i)-X(1:N));

            if( j ~= i)
                mcdsProblem.Constraints.f1(j,i) = 1 - M*(2 - X(j) - X(i)) <= sum(Z(j,:,i)) - Z(j,i,i) - Z(j,j,i) + Y(j,i);
                mcdsProblem.Constraints.f2(j,i) = sum(Z(j,:,i)) - Z(j,i,i) - Z(j,j,i) + Y(j,i)  <= 1 + M*(2-X(j)-X(i));
            end
        end
    end
    % We need at least X-1 edges for connectivty
    mcdsProblem.Constraints.a = sum(sum(a)) == sum(X)-1;

    disp(strcat("### Info: Creating optimization problem: Done!"));
    clear A adjMatrix V X Y Z
    % Save problem
    disp(strcat("### Info: Saving optimization problem: ..."));
    problem.mcdsProblem = mcdsProblem;
    problem.G = G;
    problem.comRange = comRange;
    save(exportPath,'-v7.3','problem')
    %[sol,~] = solve(mcdsProblem);
    disp(strcat("### Info: Saving optimization problem: Done!"));

    F = [1];
end

