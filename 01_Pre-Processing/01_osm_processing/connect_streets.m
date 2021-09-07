function [connectivity] = connect_streets(streets,epsilon)
    connectivity = zeros(2*length(streets));
    for srcStreet = 1:length(streets)
        for dstStreet = 1:length(streets) 
            % check distances between all four possibile connection points
            % check x1 to x1
            pointDistances = sqrt((-streets(srcStreet).x(1)+streets(dstStreet).x(1)).^2+(-streets(srcStreet).y(1)+streets(dstStreet).y(1)).^2);
            if(pointDistances < epsilon)
                connectivity(srcStreet,dstStreet+length(streets)) = sqrt((-streets(srcStreet).x(1)+streets(dstStreet).x(2)).^2+(-streets(srcStreet).y(1)+streets(dstStreet).y(2)).^2);
            end
            % check x1 to x2
            pointDistances = sqrt((-streets(srcStreet).x(1)+streets(dstStreet).x(2)).^2+(-streets(srcStreet).y(1)+streets(dstStreet).y(2)).^2);
            if(pointDistances < epsilon)
                connectivity(srcStreet,dstStreet) = sqrt((-streets(srcStreet).x(1)+streets(dstStreet).x(1)).^2+(-streets(srcStreet).y(1)+streets(dstStreet).y(1)).^2);
            end
            % check x2 to x1
            pointDistances = sqrt((-streets(srcStreet).x(2)+streets(dstStreet).x(1)).^2+(-streets(srcStreet).y(2)+streets(dstStreet).y(1)).^2);
            if(pointDistances < epsilon)
                connectivity(length(streets)+srcStreet,dstStreet+length(streets)) = sqrt((-streets(srcStreet).x(2)+streets(dstStreet).x(2)).^2+(-streets(srcStreet).y(2)+streets(dstStreet).y(2)).^2);
            end
            % check x2 to x2
            pointDistances = sqrt((-streets(srcStreet).x(2)+streets(dstStreet).x(2)).^2+(-streets(srcStreet).y(2)+streets(dstStreet).y(2)).^2);
            if(pointDistances < epsilon)
                connectivity(length(streets)+srcStreet,dstStreet) = sqrt((-streets(srcStreet).x(2)+streets(dstStreet).x(1)).^2+(-streets(srcStreet).y(2)+streets(dstStreet).y(1)).^2);
            end
        end
    end
end