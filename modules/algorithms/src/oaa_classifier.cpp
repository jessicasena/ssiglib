/*L*************************************************************************************************
*
*  IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.
*
*  By downloading, copying, installing or using the software you agree to this license. If you do
*  not agree to this license, do not download, install, copy or use the software.
*
*                            Software License Agreement (BSD License)
*                               For Smart Surveillance Framework
*                                 http://ssig.dcc.ufmg.br/ssf/
*
*  Copyright (c) 2013, Smart Surveillance Interest Group, all rights reserved.
*
*  Redistribution and use in source and binary forms, with or without modification, are permitted
*  provided that the following conditions are met:
*
*    1. Redistributions of source code must retain the above copyright notice, this list of
*       conditions and the following disclaimer.
*
*    2. Redistributions in binary form must reproduce the above copyright notice, this list of
*       conditions and the following disclaimer in the documentation and/or other materials
*       provided with the distribution.
*
*    3. Neither the name of the copyright holder nor the names of its contributors may be used to
*       endorse or promote products derived from this software without specific prior written
*       permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
*  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
*  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
*  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
*  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
*  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
*  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*************************************************************************************************L*/
#include "algorithms/oaa_classifier.hpp"

namespace ssf{


void OAAClassifier::learn(cv::Mat_<float>& input,
                          cv::Mat_<int>& labels){
  if(!mClassifiers.empty()){
    mClassifiers.clear();
  }
  samples_.release();
  labels_.release();
  labelOrderings_.clear();

  samples_ = input;
  addLabels(labels);
  int c = -1;
  for(int i = 0; i < labels.rows; ++i){
    auto label = labels[0][i];
    if(labelOrderings_.find(label)
      ==
      labelOrderings_.end()){
      labelOrderings_[label] = ++c;
    }
  }

  mClassifiers.resize(labelOrderings_.size());
  for(auto& labelIdx : labelOrderings_){
    cv::Mat_<int> localLabels = cv::Mat_<int>::zeros(samples_.rows, 1);
    for(int i = 0; i < labels.rows; ++i){
      if(labels[i][0] == labelIdx.first){
        localLabels[i][0] = 1;
      } else{
        localLabels[i][0] = -1;
      }
    }
    mClassifiers[labelIdx.second] = std::shared_ptr<Classification>
      (mUnderlyingClassifier->clone());
    mClassifiers[labelIdx.second]->learn(samples_, localLabels);
  }
  trained_ = true;
}


void OAAClassifier::predict(cv::Mat_<float>& inp,
                            cv::Mat_<float>& resp) const{
  resp = cv::Mat_<float>::zeros(inp.rows, static_cast<int>(mClassifiers.size()));

  for(int r = 0; r < inp.rows; ++r){
    int c = 0;
    for(auto& classifier : mClassifiers){
      cv::Mat_<float> auxResp;
      classifier->predict(inp, auxResp);
      auto ordering = classifier->getLabelsOrdering();
      const int idx = ordering[1];
      resp[r][c] = auxResp[0][idx];
      ++c;
    }
  }
}


cv::Mat_<int> OAAClassifier::getLabels() const{
  return labels_;
}


std::unordered_map<int, int> OAAClassifier::getLabelsOrdering() const{
  return labelOrderings_;
}

bool OAAClassifier::empty() const{
  return mClassifiers.empty();
}


bool OAAClassifier::isTrained() const{
  return trained_;
}


bool OAAClassifier::isClassifier() const{
  return true;
}


void OAAClassifier::load(const std::string& filename, const std::string& nodename){
  //TODO:
}


void OAAClassifier::save(const std::string& filename, const std::string& nodename) const{
  //TODO:
}

Classification* OAAClassifier::clone() const{
  auto copy = new OAAClassifier;
  copy->setMaxIterations(getMaxIterations());
  copy->setEpsilon(getEpsilon());
  copy->setTermType(getTermType());
  copy->setUnderlyingClassifier(*getUnderlyingClassifier());
  copy->setClassWeights(getClassWeights());
  return copy;
}

std::shared_ptr<Classification> OAAClassifier::getUnderlyingClassifier() const{
  return std::shared_ptr<Classification>(mUnderlyingClassifier->clone());;
}

void OAAClassifier::setUnderlyingClassifier(const Classification& underlyingClassifier){
  mUnderlyingClassifier = std::unique_ptr<Classification>(underlyingClassifier.clone());
}

void OAAClassifier::addLabels(cv::Mat_<int>& labels){
  labels_.release();
  labels_ = labels;
}

}
