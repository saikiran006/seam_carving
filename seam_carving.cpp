//compile using g++ -o seam_carving.out seam_carving.cpp `pkg-config --cflags --libs opencv4`

#include <opencv2/opencv.hpp>
#include <iostream>

using namespace cv;
using namespace std;

struct DescendingOrder {
    bool operator()(const int& lhs, const int& rhs) const {
        return lhs > rhs;
    }
};

double calculateGradient(Mat image, int x, int y, int h, int w){
    int left=(y+w-1)%w,right=(y+w+1)%w,top=(x+h-1)%h,bottom=(x+h+1)%h;
    double x1=image.at<Vec3b>(x, left)[0]-image.at<Vec3b>(x, right)[0];
    double x2=image.at<Vec3b>(x, left)[1]-image.at<Vec3b>(x, right)[1];
    double x3=image.at<Vec3b>(x, left)[2]-image.at<Vec3b>(x, right)[2];
    double y1=image.at<Vec3b>(top, y)[0] - image.at<Vec3b>(bottom, y)[0];
    double y2=image.at<Vec3b>(top, y)[1] - image.at<Vec3b>(bottom, y)[1];
    double y3=image.at<Vec3b>(top, y)[2] - image.at<Vec3b>(bottom, y)[2];
    double xGrad=x1*x1+x2*x2+x3*x3, yGrad=y1*y1+y2*y2+y3*y3;
    return sqrt(xGrad+yGrad);the
} 

vector<int> findSeam(int h, int w, string direction,vector<vector<double>> &gradient){
    vector<vector<double>> dp(h,vector<double>(w));
    if(direction == "vertical"){
        vector<int> seam(h);
        for(int j=0;j<w;j++){
            dp[0][j]=gradient[0][j];
        }
        for(int i=1;i<h;i++){
            for(int j=0;j<w;j++){
                double minGrad=dp[i-1][j];
                if(j>0) minGrad=min(minGrad,dp[i-1][j-1]);
                if(j<w-1) minGrad=min(minGrad,dp[i-1][j+1]);
                dp[i][j]=gradient[i][j]+minGrad;
            }
        }
        int minj=0;
        for(int j=1;j<w;j++){
            if(dp[h-1][minj]>dp[h-1][j]) minj=j;
        }
        seam[h-1]=minj;
        for(int i=h-2;i>=0;i--){
            if(minj>0 && dp[i][minj]>dp[i][minj-1]) minj=minj-1;
            if(minj<w-1 && dp[i][minj]>dp[i][minj+1]) minj=minj+1;
            seam[i]=minj;
        }
        return seam;
    }
    else{
        vector<int> seam(w);
        for(int i=0;i<h;i++){
            dp[i][0]=gradient[i][0];
        }
        for(int j=1;j<w;j++){
            for(int i=0;i<h;i++){
                double minGrad=dp[i][j-1];
                if(i>0) minGrad=min(minGrad,dp[i-1][j-1]);
                if(i<h-1) minGrad=min(minGrad,dp[i+1][j-1]);
                dp[i][j]=gradient[i][j]+minGrad;
            }
        }
        int mini=0;
        for(int i=0;i<h;i++){
            if(dp[i][w-1]<dp[mini][w-1]) mini=i;
        }
        seam[w-1]=mini;
        for(int j=w-2;j>=0;j--){
            if(mini>0 && dp[mini][j]>dp[mini-1][j]) mini=mini-1;
            if(mini<h-1 && dp[mini][j]>dp[mini+1][j]) mini=mini+1;
            seam[j]=mini;
        }
        return seam;
    }
}


int main()
{
    string str;
    cin>>str;
    Mat image = imread(str);
    int h = image.rows;
    int w = image.cols;
    cout<<"Height and Width of the source image are "<<h<<" & "<<w<<" pixels"<<endl;
    cout<<"Enter new height and width in pixels"<<endl;
    int new_h, new_w;
    cin >> new_h >> new_w;
    // vector<int> horizontalSeam,verticalSeam;
    vector<vector<double>> gradient(h,vector<double>(w));
    
    //Finding the energy for the first time
    for(int i=0;i<h;i++){
        for(int j=0;j<w;j++){
            gradient[i][j]=calculateGradient(image,i,j,h,w);
        }
    }
    if(w>new_w){
        for (int numTimes = 0; numTimes < w - new_w; numTimes++)
        {
            vector<int> verticalSeam = findSeam(h, w, "vertical",gradient);
            for(int i=0;i<h;i++){
                for(int j=verticalSeam[i];j<w-1;j++){
                    image.at<Vec3b>(i,j)=image.at<Vec3b>(i,j+1);
                    gradient[i][j]=gradient[i][j+1];
                }
                // image.at<Vec3b>(i,verticalSeam[i])=0;
            }
            w = w - 1;
            for (int i = 0; i < h; i++) {
                int curr = verticalSeam[i];
                if (curr > 0) {
                    gradient[i][curr - 1] = calculateGradient(image, i, curr - 1, h, w);
                }
                else if(curr==0){
                    gradient[i][w-1] = calculateGradient(image,i,w-1,h,w);
                }
                if (curr < w) {
                    gradient[i][curr] = calculateGradient(image, i, curr, h, w);
                }
            }
        }
            
    }
    else if(w<new_w){
        int k = new_w - w;
        vector<vector<int>> seams;
        for (int numTimes = 0; numTimes < k; numTimes++) {
            vector<int> verticalSeam = findSeam(h, w, "vertical", gradient);
            seams.push_back(verticalSeam);
            for (int i = 0; i < h; i++) {
                // image.at<Vec3b>(i, verticalSeam[i]) = 0;
                gradient[i][verticalSeam[i]] = 625;
            }
        }

        vector<set<int, DescendingOrder>> sortedSeams(h, set<int, DescendingOrder>(DescendingOrder()));
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < h; j++) {
                sortedSeams[j].insert(seams[i][j]);
            }
        }

        Mat newImage(h, w + k, image.type());

        for (int i = 0; i < h; i++) {
            int f = k;
            auto it = sortedSeams[i].begin();

            for (int j = w - 1; j >= 0; j--) {
                if (f > 0 && it != sortedSeams[i].end() && j == *it) {
                    Vec3b avgPixel = image.at<Vec3b>(i, j);
                    if (j < w - 1) {
                        Vec3b nextPixel = image.at<Vec3b>(i, j + 1);
                        for (int x = 0; x < 3; x++) {
                            avgPixel[x] = (avgPixel[x] + nextPixel[x]) / 2;
                        }
                    }

                    newImage.at<Vec3b>(i, j + f) = avgPixel;
                    newImage.at<Vec3b>(i, j + f - 1) = image.at<Vec3b>(i, j);

                    f--;
                    it++;
                } else {
                    newImage.at<Vec3b>(i, j + f) = image.at<Vec3b>(i, j);
                }
            }
            gradient[i].resize(w+k);
        }
        // Mat tempImage=image;
        image=newImage;
        // tempImage.release();
        w+=k;
        
        for(int i=0;i<h;i++){
            for(int j=0;j<w;j++){
                gradient[i][j]=calculateGradient(image,i,j,h,w);
            }
        }
    }
    if(h>new_h){
        for (int numTimes = 0; numTimes < abs(h - new_h); numTimes++)
        {
            vector<int> horizontalSeam = findSeam(h, w, "horizontal",gradient);
            for(int j=0;j<w;j++){
                for(int i=horizontalSeam[j];i<h-1;i++){
                    image.at<Vec3b>(i,j)=image.at<Vec3b>(i+1,j);
                    gradient[i][j]=gradient[i+1][j];
                }
                // image.at<Vec3b>(horizontalSeam[j],j)=0;
            }
            h = h - 1;
            for (int j = 0; j < w; j++) {
                int curr = horizontalSeam[j];
                if (curr > 0) {
                    gradient[curr - 1][j] = calculateGradient(image, curr - 1, j, h, w);
                }
                else if(curr==0){
                    gradient[h-1][j] = calculateGradient(image, h-1,j,j,w);
                }
                if (curr < h) {
                    gradient[curr][j] = calculateGradient(image, curr, j, h, w);
                }
            }
        }
    }
    else if(h<new_h){
        int k = new_h - h;
        vector<vector<int>> seams;
        for (int numTimes = 0; numTimes < k; numTimes++) {
            vector<int> horizontalSeam = findSeam(h, w, "horizontal", gradient);
            seams.push_back(horizontalSeam);
            for (int j = 0; j < w; j++) {
                // image.at<Vec3b>(horizontalSeam[j], j) = 0;
                gradient[horizontalSeam[j]][j] = 625;
            }
        }

        vector<set<int, DescendingOrder>> sortedSeams(w, set<int, DescendingOrder>(DescendingOrder()));
        for (int i = 0; i < k; i++) {
            for (int j = 0; j < w; j++) {
                sortedSeams[j].insert(seams[i][j]);
            }
        }

        Mat newImage(h+k, w, image.type());
        for(int j = 0; j < w; j++) {
            int f=k;
            auto it=sortedSeams[j].begin();
            for(int i=h-1;i>=0;i--){
                if(f>0 && it!=sortedSeams[j].end() && i==*it){
                    Vec3b avgPixel = image.at<Vec3b>(i, j);
                    if (i < h - 1) {
                        Vec3b nextPixel = image.at<Vec3b>(i+1, j);
                        for (int x = 0; x < 3; x++) {
                            avgPixel[x] = (avgPixel[x] + nextPixel[x]) / 2;
                        }
                    }

                    newImage.at<Vec3b>(i+f, j) = avgPixel;
                    newImage.at<Vec3b>(i+f-1, j) = image.at<Vec3b>(i, j);

                    f--;
                    it++;
                }
                else {
                    newImage.at<Vec3b>(i+f, j) = image.at<Vec3b>(i, j);
                }
            }
        }
        // Mat tempImage=image;
        image=newImage;
        // tempImage.release();
        h+=k;
    }
    image.cols = w;
    image.rows = h;
    imshow("result", image);
    imwrite("Output.jpg",image);
    waitKey(0);

    destroyWindow("Result");
    return 0;
}

