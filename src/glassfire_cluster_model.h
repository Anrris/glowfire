#ifndef CENTROID_MODEL_H
#define CENTROID_MODEL_H

#include "glassfire_base.h"

namespace glassfire{

template<typename AxisType, typename FeatureInfo>
class ClusterModel{
public:
    typedef Eigen::Matrix<AxisType, Eigen::Dynamic, Eigen::Dynamic>               Matrix;
    typedef Eigen::Matrix<AxisType, 1, Eigen::Dynamic>               RowVector;
    typedef Eigen::Matrix<AxisType, Eigen::Dynamic, 1>               ColVector;
    typedef std::vector<AxisType>    Feature;
private:
    AxisType  _pi_ = 3.141592653589793;
    std::string mModelKey;
    std::vector<size_t> m_in_range_feature_s_index;

    RowVector   mRowMean;
    Feature     mMean;

    Matrix      mCmat, mRegularizedCmat;
    AxisType    mCmatRegularize;
    Matrix      mInvCmat;
    AxisType    mCmatDet;
    size_t      Dimension;


    auto set_parameters(const Feature & mean, const Matrix & cmat, const AxisType &cmat_regularize) -> void {
        Dimension = mean.size();
        mRowMean = RowVector(1, Dimension);
        for(size_t i=0; i<mean.size(); i++)
            mRowMean(0, i) = mean[i];
        mMean = mean;
        mCmat = cmat;
        mCmatRegularize = cmat_regularize;
        mRegularizedCmat = mCmat;

        for(long i=0; i<mCmat.cols(); i++){
            mRegularizedCmat(i,i) += cmat_regularize;
        }
        mInvCmat = mRegularizedCmat.inverse();
        mCmatDet = mRegularizedCmat.determinant();
    }
public:
    ClusterModel(){}

    ClusterModel(
        const Feature & mean,
        const Matrix & cmat,
        const std::string & modelKey,
        const std::vector<size_t> & in_range_feature_s_index,
        AxisType cmat_regularize
        ):  mModelKey(modelKey),
            m_in_range_feature_s_index(in_range_feature_s_index),
            mCmatRegularize(cmat_regularize)
    {
        set_parameters(mean, cmat, cmat_regularize);
    }
    auto eval(Feature feature) -> AxisType {
        auto rowVec = feature_sub_mean_to_rowVector(feature);
        auto colVec = feature_sub_mean_to_colVector(feature);
        AxisType mahalanDistance = (rowVec * mInvCmat * colVec)(0, 0);
        AxisType result;
        result = (AxisType)exp( -0.5* mahalanDistance) / sqrt( pow(2 * _pi_, feature.size()) * mCmatDet );
        return result;
    }
    auto set_regularize(AxisType cmat_regularize) -> void {
        set_parameters(mMean, mCmat, cmat_regularize);
    }
    auto count()-> const size_t {return m_in_range_feature_s_index.size();}
    auto cmean()-> const RowVector & {return mRowMean;}
    auto mean() -> const Feature & {return mMean;}
    auto cov_mat() -> const Matrix & {return mRegularizedCmat;}
    auto model_key() -> const string & {return mModelKey;}
    auto get_data_index() -> const std::vector<size_t> & {return m_in_range_feature_s_index;}

private:
    auto feature_sub_mean_to_rowVector(const Feature &feature) -> Matrix {
        Matrix rowVector(1, Dimension);
        for(size_t i=0; i<feature.size(); i++){
            rowVector(0,i) = feature[i] - mMean[i];
        }
        return rowVector;
    }
    auto feature_sub_mean_to_colVector(const Feature &feature) -> Matrix {
        Matrix colVector(Dimension, 1);
        for(size_t i=0; i<feature.size(); i++){
            colVector(i,0) = feature[i] - mMean[i];
        }
        return colVector;
    }
};

}


#endif