#include "CellularAutomata.h"

#include "Random.h"
#include "Neighbourhood.h"

#include <QMouseEvent>
#include <QPainter>

CellularAutomata::CellularAutomata(QWidget* parent, unsigned rows, unsigned columns)
    : QWidget(parent)
    , cells(rows, std::vector<double>(columns))
    , nextCells(rows, std::vector<double>(columns))
    , stepCell_(GetDefaultCellStepper())
    , colouriser_(GetDefaultCellColouriser())
{
}

void CellularAutomata::Step()
{
    for (size_t row = 0; row < cells.size(); row++) {
        // TODO calculate each row in threadpool
        for (size_t column = 0; column < cells[row].size(); column++) {
            GetNeighbourFunc getNeighbourFunc = [&](int offsetX, int offsetY) -> const double& { return GetCellValue(row, column, offsetX, offsetY); };
            nextCells[row][column] = stepCell_(getNeighbourFunc);
        }
    }
    std::swap(cells, nextCells);
    update();
}

const double& CellularAutomata::GetCellValue(size_t x, size_t y, int offsetX, int offsetY) const
{
    int64_t intX = x + offsetX;
    int64_t intY = y + offsetY;
    return cells[intX % cells.size()][intY % cells[x].size()];
}

void CellularAutomata::Clear(double value)
{
    for (unsigned x = 1; x < cells.size() - 1; x++) {
        for (unsigned y = 1; y < cells[x].size() - 1; y++) {
            cells[x][y] = value;
            nextCells[x][y] = value;
        }
    }
}

std::function<double (const std::function<const double& (int, int)>& getCellValue)> CellularAutomata::GetDefaultCellStepper() const
{
    // Conways game of life
    return [](const GetNeighbourFunc& getCellValue) -> double
    {
        static auto neighbourhood = Neighbourhood::CreateNeighbourhoodCoordinates(
                                    {
                                        { 1, 1, 1 },
                                        { 1, 0, 1 },
                                        { 1, 1, 1 },
                                    });
        double neighbours = 0.0;
        for (auto coord : neighbourhood) {
            neighbours += getCellValue(coord.first, coord.second);
        }

        if (neighbours == 3 || (getCellValue(0, 0) != 0.0 && neighbours == 2.0)) {
            return 1.0;
        } else {
            return 0.0;
        }
    };
}

std::function<unsigned (const double& value)> CellularAutomata::GetDefaultCellColouriser() const
{
    return [](const double& value) -> unsigned
    {
        return value == 0 ? 0x00FFFFFF : 0x00000000;
    };
}

void CellularAutomata::SetCellStepper(std::function<double(const GetNeighbourFunc&)>&& stepper)
{
    stepCell_ = std::move(stepper);
}

void CellularAutomata::SetCellColouriser(std::function<unsigned (const double&)>&& converter)
{
    colouriser_ = std::move(converter);
    update();
}

void CellularAutomata::wheelEvent(QWheelEvent* event)
{
    double d = 1.0 + (0.001 * double(event->angleDelta().y()));
    scale_ *= d;
    update();
}

void CellularAutomata::paintEvent(QPaintEvent* /*event*/)
{
    QPainter p(this);
    p.translate(0.0 + (width() / 2.0), 0.0 + (height() / 2.0));
    p.scale(scale_, scale_);
    p.translate(0.0 - (Rows() / 2.0), 0.0 - (Columns() / 2.0));
    for (unsigned x = 0; x < cells.size(); x++) {
        for (unsigned y = 0; y < cells[x].size(); y++) {
            p.setPen(colouriser_(cells[x][y]));
            p.drawPoint(x, y);
        }
    }
}
