#ifndef CELLULARAUTAMATA_H
#define CELLULARAUTAMATA_H

#include "Random.h"
#include "NeuralNetwork.h"

#include <vector>
#include <functional>
#include <time.h>

#include <QWidget>

class QPainter;

class CellularAutomata : public QWidget {
    Q_OBJECT
public:
    using GetNeighbourFunc = std::function<const double& (int xOffset, int yOffset)>;

    CellularAutomata(QWidget* parent, unsigned rows = 100, unsigned columns = 100);

    void Step();
    void Paint(QPainter& p) const;

    size_t Rows() const { return cells.front().size(); }
    size_t Columns() const { return cells.size(); }

    const double& GetCellValue(size_t x, size_t y, int offsetX = 0, int offsetY = 0) const;

    void Clear(double value = 0.0);
    void SetDimensions(size_t width, size_t height)
    {
        cells.resize(height, std::vector<double>(width, 0.0));
        nextCells.resize(height, std::vector<double>(width, 0.0));
        for (auto& row : cells) {
            row.resize(width, 0.0);
        }
        for (auto& row : nextCells) {
            row.resize(width, 0.0);
        }
    }
    template <typename T>
    void Randomise(T min, T max)
    {
        Random::Seed(static_cast<unsigned long>(time(nullptr)));

        for (unsigned x = 0; x < cells.size(); x++) {
            for (unsigned y = 0; y < cells[x].size(); y++) {
                cells[x][y] = static_cast<double>(Random::Number<T>(min, max));
            }
        }
        update();
    }

    std::function<double (const GetNeighbourFunc& getCellValue)> GetDefaultCellStepper() const;
    std::function<unsigned(const double& value)> GetDefaultCellColouriser() const;

    void SetCellStepper(std::function<double(const GetNeighbourFunc& getCellValue)>&& stepper);
    void SetCellColouriser(std::function<unsigned(const double& value)>&& converter);

protected:
    virtual void wheelEvent(QWheelEvent* event) override final;
    virtual void paintEvent(QPaintEvent* event) override final;

private:
    double scale_ = 1.0;
    unsigned fps_ = 5;

    std::vector<std::vector<double>> cells;
    std::vector<std::vector<double>> nextCells;

    std::function<double(const GetNeighbourFunc& getCellValue)> stepCell_;
    std::function<unsigned(const double& value)> colouriser_;
};

#endif // CELLULARAUTAMATA_H
