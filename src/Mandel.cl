#pragma OPENCL EXTENSION cl_khr_gl_event : enable
__kernel void mandel(write_only image2d_t writeToImage,                             
                     double minX,                                                  
                     double maxX,                                                  
                     double minY)                                                  
        {                                                               
            // get id of element in array                                   
            uint x = get_global_id(0);                                      
            uint y = get_global_id(1);                                      
            uint w = get_global_size(0);                                    
            uint h = get_global_size(1);                                    
            float4 result = (float4)(0.0f, 0.0f, 0.0f, 1.0f);               
            double MaxIm = minY+(maxX-minX)*h/w;                        
            double Re_factor = (maxX-minX)/(w-1);                          
            double Im_factor = (MaxIm-minY)/(h-1);                       
            const float maxIter = 10000.0f;
            
            // declare 4 bands of colour gradient to be used for low to high iterations
            bool bColBand1 = false;
            bool bColBand2 = false;
            bool bColBand3 = false;
            bool bColBand4 = false;  
            bool bColBand5 = false;
            bool bColBand6 = false; 
            
            // colour min and max per band
            float4 white =       {1.0f, 1.0f, 1.0f, 1.0f};
            float4 lightOrange = {255.0f, 200.0f, 50.0f,   1.0f};
            float4 darkOrange =  {255.0f, 153.0f, 20.0f,   1.0f};
            float4 teal =        {51.0f,  201.0f, 153.0f, 1.0f};
            float4 tealBright =  {50.0f,   255.0f, 255.0f, 1.0f};
            float4 greenOlive =  {153.0f, 204.0f, 20.0f,   1.0f};
            float4 gold =        {255.0f, 255.0f, 20.0f,   1.0f};
            float4 teal2 =       {50.0f,   153.0f, 153.0f, 1.0f};
            float4 lightRed =    {255.0f, 51.0f,  51.0f,   1.0f};
            float4 darkRed =     {102.0f, 10.0f,  10.0f,   1.0f};
            lightOrange = lightOrange / 255.0f;
            darkOrange = darkOrange / 255.0f;
            teal   = teal   / 255.0f;
            tealBright = tealBright / 255.0f;
            teal2   = teal2   / 255.0f;
            greenOlive = greenOlive / 255.0f;
            gold = gold / 255.0f;
            lightRed = lightRed / 255.0f;
            darkRed = darkRed / 255.0f;
            // C imaginary, C real, Z real                                                  
            double c_im = MaxIm - y*Im_factor;                           
            double c_re = minX + x*Re_factor;                            
            double Z_re = c_re, Z_im = c_im;                             
            bool isInside = true;                                        
            int iter = 0; 
            
            for(int n=0; n<maxIter; n++)                                    
            {                                                               
                // Z - real and imaginary                                   
                double Z_re2 = Z_re*Z_re;
                double Z_im2 = Z_im*Z_im;                
                //if Z real squared plus Z imaginary squared is greater than c squared  \n"
                if(Z_re2 + Z_im2 > 4)                                       
                {                                                           
                    if(n >= 0 && n <= (maxIter/200-1))                        
                    {   
                        bColBand1 = true;                        
                        isInside = false;                                   
                        break;                                              
                    }
                    else if(n >= maxIter/200 && n <= (maxIter/100-1))                        
                    {   
                        bColBand2 = true;                        
                        isInside = false;                                   
                        break;                                              
                    }
                    else if(n >= maxIter/100 && n <= (maxIter/50-1))                        
                    {   
                        bColBand3 = true;                        
                        isInside = false;                                   
                        break;                                              
                    }
                    else if(n >= maxIter/50 && n <= (maxIter/25-1))                        
                    {   
                        bColBand4 = true;                        
                        isInside = false;                                   
                        break;                                              
                    }
                    else if(n >= maxIter/25 && n <= (maxIter/7-1))                        
                    {   
                        bColBand5 = true;                        
                        isInside = false;                                   
                        break;                                              
                    }
                    else if(n >= maxIter/7 && n <= (maxIter-1))                        
                    {   
                        bColBand6 = true;                        
                        isInside = false;                                   
                        break;                                              
                    }
                }                                                         
                Z_im = 2*Z_re*Z_im + c_im;                                  
                Z_re = Z_re2 - Z_im2 + c_re;                                
                iter++;                                                     
            }                                                             
            //histogram[iter] +=1;                                            
            //xyIter[x][y] = iter;
            if(bColBand1)                                                        
            {                                                               
                result = iter * teal / 50.0f;
                result = (float4){result.x, result.y, result.z, 1.0f};
            }
            if(bColBand2)                                                        
            {                                                               
                result = darkOrange - (iter * darkOrange / 150.0f);
                result = (float4){result.x, result.y, result.z, 1.0f};
            }
            else if(bColBand3)                                                        
            {                                                               
                result = greenOlive - (iter * greenOlive / 200.0f);
                result = (float4){result.x, result.y, result.z, 1.0f};
            }
            else if(bColBand4)                                                        
            {                                                               
                result = gold - (iter * gold / 400.0f);
                result = (float4){result.x, result.y, result.z, 1.0f};
            }
            else if(bColBand5)                                                        
            {                                                               
                result = teal - (iter * teal / 2000.0f);
                result = (float4){result.x, result.y, result.z, 1.0f};
            }
            else if(bColBand6)                                                        
            {                                                               
                result = lightRed - (iter * lightRed / 4000.0f);
                result = (float4){result.x, result.y, result.z, 1.0f};
            }
            else if(isInside)                                               
            {                                                               
                result = (float4)(0.0f, 0.0f, 0.0f, 1.0f);                         
            }
            write_imagef(writeToImage, (int2)(x, y), result);               
        }                                                                
