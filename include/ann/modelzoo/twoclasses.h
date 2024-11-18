/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt
 * to change this license Click
 * nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this
 * template
 */

/*
 * File:   twoclasses.h
 * Author: ltsach
 *
 * Created on October 8, 2024, 8:42 PM
 */

#ifndef TWOCLASSES_H
#define TWOCLASSES_H
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
using namespace std;

#include "ann/annheader.h"
#include "config/Config.h"
#include "dataset/DSFactory.h"
#include "loader/dataloader.h"
#include "loader/dataset.h"
#include "sformat/fmt_lib.h"
#include "tensor/xtensor_lib.h"

void twoclasses_classification() {
  DSFactory factory("./config.txt");
  xmap<string, TensorDataset<double, double>*>* pMap =
      factory.get_datasets_2cc();
  TensorDataset<double, double>* train_ds = pMap->get("train_ds");
  TensorDataset<double, double>* valid_ds = pMap->get("valid_ds");
  TensorDataset<double, double>* test_ds = pMap->get("test_ds");
  DataLoader<double, double> train_loader(train_ds, 50, true, false);
  DataLoader<double, double> valid_loader(valid_ds, 50, false, false);
  DataLoader<double, double> test_loader(test_ds, 50, false, false);

  int nClasses = 2;
  ILayer* layers[] = {new FCLayer(2, 50, true),        new ReLU(),
                      new FCLayer(50, 20, true),       new ReLU(),
                      new FCLayer(20, nClasses, true), new Softmax()};
  MLPClassifier model("./config.txt", "2c-classification", layers,
                      sizeof(layers) / sizeof(ILayer*));

  SGD optim(2e-3);
  CrossEntropy loss;
  ClassMetrics metrics(nClasses);
  model.compile(&optim, &loss, &metrics);
  MLPClassifier pretrained1("./config.txt");
  string base_path = "./models";
  pretrained1.load(base_path + "/" + "2c-classification-1", true);

  // test funtion evaluate
  cout << "--------------- evaluate -----------" << endl;
  double_tensor eval_rs1 = pretrained1.evaluate(&test_loader);
  double_tensor expect_1 = {0.995, 0.995,    0.9952,  0.995192,
                            0.995, 0.994998, 0.995002};
  cout << "ouput you : " << endl;
  cout << eval_rs1 << endl;
  cout << "expect : " << endl;
  cout << expect_1 << endl;
  cout << endl;

  // test funtion predict double_tensor
  cout << "--------------- predict double_tensor -----------" << endl;
  double_tensor X_double_tensor = {0.93980859328292, 0.4571194992357245};
  double_tensor predict_double_tensor =
      pretrained1.predict(X_double_tensor, true);
  double_tensor expect_2 = {0.994444, 0.005556};
  cout << "ouput you : " << endl;
  cout << predict_double_tensor << endl;
  cout << "expect : " << endl;
  cout << expect_2 << endl;
  cout << endl;

  // test funtion predict DataLoader
  double_tensor predict_DataLoader = pretrained1.predict(&test_loader, false);
  double_tensor expect_3 = {
      0., 0., 0., 1., 1., 1., 0., 1., 1., 0., 0., 1., 1., 0., 1., 1., 0.,
      1., 0., 0., 1., 0., 1., 0., 0., 0., 0., 0., 1., 1., 0., 0., 0., 0.,
      1., 1., 1., 1., 1., 0., 0., 1., 0., 0., 1., 1., 0., 0., 1., 0., 1.,
      1., 0., 1., 1., 0., 1., 0., 1., 0., 1., 0., 0., 0., 0., 1., 0., 1.,
      0., 1., 0., 1., 1., 0., 1., 1., 0., 1., 1., 1., 0., 0., 0., 1., 1.,
      0., 1., 0., 1., 0., 0., 1., 1., 0., 0., 1., 1., 0., 0., 1., 0., 0.,
      1., 1., 1., 1., 0., 0., 1., 1., 1., 0., 1., 1., 0., 1., 0., 0., 1.,
      0., 0., 1., 1., 1., 0., 0., 1., 1., 0., 0., 1., 0., 0., 1., 0., 1.,
      0., 1., 0., 1., 1., 1., 1., 0., 0., 0., 0., 1., 0., 0., 1., 1., 0.,
      0., 0., 0., 1., 1., 0., 0., 1., 1., 1., 1., 0., 0., 1., 0., 1., 0.,
      0., 0., 0., 0., 1., 1., 1., 0., 1., 1., 0., 0., 1., 1., 1., 1., 0.,
      1., 0., 0., 1., 1., 0., 1., 0., 1., 0., 0., 1., 1.};
  cout << "predict : ";
  for (const auto& val : predict_DataLoader) {
    std::cout << val << " ";
  }
  cout << endl;

  cout << "expect  : ";
  for (const auto& val : expect_3) {
    std::cout << val << " ";
  }
  cout << endl;

  int correct = 0;
  for (size_t i = 0; i < predict_DataLoader.size(); ++i) {
    if (predict_DataLoader(i) == expect_3(i)) {
      ++correct;
    }
  }

  // 0.995
  std::cout << "Accuracy: "
            << static_cast<double>(correct) / predict_DataLoader.size()
            << std::endl;
  std::cout << "expect Accuracy: " << 0.995 << std::endl;
}

#endif /* TWOCLASSES_H */