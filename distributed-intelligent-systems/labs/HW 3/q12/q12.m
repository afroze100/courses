clear
clc
time_global = 0;

for iters=1:10000
    robots = zeros(1,15);
    
    right = 0;
    left = 0;
    exit = 0;
    time_local = 0;

    for i=1:15
        if rand(1) >= 0.5
            robots(i) = 2;
            right = right + 1;
        else
            robots(i) = 1;
            left = left + 1;
        end
    end



    while exit == 0
        if right > left
            for i=1:15
                if robots(i) == 1
                    if rand(1) >= 0.5
                        robots(i) = 2;
                    end
                end
            end
        else
            for i=1:15
                if robots(i) == 2
                    if rand(1) >= 0.5
                        robots(i) = 1;
                    end
                end
            end
        end
        
        right = 0;
        left = 0;
        
        for i=1:15
            if robots(i) == 1
                left = left + 1;
            else
                right = right + 1;
            end
        end
        
        if (right == 0) || (left == 0)
            exit = 1;
        end
        
        time_local = time_local + 1;
    end

    time_global = time_global + time_local;
    
end

time_global = time_global / iters;
time_global
    
   
    
    



        