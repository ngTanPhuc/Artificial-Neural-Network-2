/*
 * Click nbfs://nbhost/SystemFileSystem/Templates/Licenses/license-default.txt to change this license
 * Click nbfs://nbhost/SystemFileSystem/Templates/cppFiles/file.h to edit this template
 */

/*
 * File:   dataset.h
 * Author: ltsach
 *
 * Created on September 2, 2024, 3:59 PM
 */

#ifndef DATASET_H
#define DATASET_H
#include "tensor/xtensor_lib.h"
using namespace std;

template <typename DType, typename LType>
class DataLabel
{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;

public:
    DataLabel(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label)
    {
    }
    xt::xarray<DType> getData() const { return data; }
    xt::xarray<LType> getLabel() const { return label; }
};

template <typename DType, typename LType>
class Batch
{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;

public:
    Batch() = default;
    Batch(xt::xarray<DType> data, xt::xarray<LType> label) : data(data), label(label)
    {
    }
    virtual ~Batch() {}
    xt::xarray<DType> &getData() { return data; }
    xt::xarray<LType> &getLabel() { return label; }
    bool operator==(const Batch<DType, LType> &other) const
    {
        return (this->data == other.data) && (this->label == other.label);
    }
    friend std::ostream &operator<<(std::ostream &os, const Batch<DType, LType> &batch)
    {
        os << "Data:\n"
           << batch.data << "\n";
        os << "Label:\n"
           << batch.label << "\n";
        return os;
    }
};

template <typename DType, typename LType>
class Dataset
{
private:
public:
    Dataset() {};
    virtual ~Dataset() {};

    virtual int len() = 0;
    virtual DataLabel<DType, LType> getitem(int index) = 0;
    virtual xt::svector<unsigned long> get_data_shape() = 0;
    virtual xt::svector<unsigned long> get_label_shape() = 0;
};

//////////////////////////////////////////////////////////////////////
template <typename DType, typename LType>
class TensorDataset : public Dataset<DType, LType>
{
private:
    xt::xarray<DType> data;
    xt::xarray<LType> label;
    xt::svector<unsigned long> data_shape, label_shape;

public:
    /* TensorDataset:
     * need to initialize:
     * 1. data, label;
     * 2. data_shape, label_shape
     */
    TensorDataset(xt::xarray<DType> data, xt::xarray<LType> label)
    {
        /* TODO: your code is here for the initialization
         */
        this->data = data;
        this->label = label;
        label_shape = xt::svector<unsigned long>(label.shape().begin(), label.shape().end());
        data_shape = xt::svector<unsigned long>(data.shape().begin(), data.shape().end());
    }
    /* len():
     *  return the size of dimension 0
     */
    int len()
    {
        /* TODO: your code is here to return the dataset's length
         */
        return data.shape()[0]; // remove it when complete
    }

    /* getitem:
     * return the data item (of type: DataLabel) that is specified by index
     */
    DataLabel<DType, LType> getitem(int index)
    {
        /* TODO: your code is here
         */
        if (index < 0 || index >= len())
        {
            throw std::out_of_range("Index is out of range!");
        }
        xt::xarray<DType> sample_data;
        xt::xarray<LType> sample_label;
        if (label.shape().size() == 0)
        {
            sample_label = label;
        }
        else
        {
            sample_label = view(label, index);
        }
        if (data.shape().size() == 0)
        {
            sample_data = data;
        }
        else
        {
            sample_data = view(data, index);
        }
        return DataLabel<DType, LType>(sample_data, sample_label);
    }

    xt::svector<unsigned long> get_data_shape()
    {
        /* TODO: your code is here to return data_shape
         */
        return data_shape;
    }
    xt::svector<unsigned long> get_label_shape()
    {
        /* TODO: your code is here to return label_shape
         */
        return label_shape;
    }
};

#endif /* DATASET_H */
