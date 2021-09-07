function [feederInfo] = create_kmedoid_feeder(mean_num_loads, km_num_replicates, km_opts, graph, buildingInfo)

    XY = [buildingInfo.cX;buildingInfo.cY]';
    numFeeder = floor(length(XY)/mean_num_loads);

    distance = @(X,obj)graph_distance(graph,buildingInfo,X,obj);
    [idx,C] = kmedoids(XY,numFeeder,'Replicates',km_num_replicates,'Options',km_opts,'Distance',distance,'Algorithm','large','Start','sample');

    feederInfo.idx = idx;
    feederInfo.C = C;
end

