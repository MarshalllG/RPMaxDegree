// RP_Greedy.cc 

#include "RP_Greedy.hh"
#include "Random.hh"
#include <algorithm>
#include <numeric>

using namespace std;


bool PointIsInVec (point pt, vector<point> vec) 
{
    for (unsigned i = 0; i < vec.size(); i++)
        if (pt.row == vec[i].row && pt.col == vec[i].col)
            return 1;

    return 0;
}


void PrintVec (vector<point> vec) 
{
    for (unsigned i = 0; i < vec.size(); i++)
    {
        cout << vec[i].row << " " << vec[i].col;
        if (i != vec.size()-1)
            cout << " -> ";
    }
        
    cout << endl;
}

bool IsFeasibleRouter (const RP_Input& in, RP_Output& out, point r)
{
    return (in.IsDot(r.row, r.col) || (in.RouterPrice() + (ManhattanDistance(r, out.ClosestBackbonePoint(r)) * in.BackbonePrice()) >= out.RemainingBudget()));
}


void ConnectRouterWithBackbone(RP_Output& out, const RP_Input& in, point router)
{
    point temp, selected_bb_cell;
    unsigned new_distance;

    // select the backbone point
    selected_bb_cell = out.ClosestBackbonePoint(router);
    // cout << "selectedbbcell: " << selected_bb_cell.row << " " << selected_bb_cell.col << endl;

    unsigned current_distance = ManhattanDistance(selected_bb_cell, router);

    while (current_distance > 0)
    {
        temp = MakeRandomMove(selected_bb_cell);
        new_distance = ManhattanDistance(temp, router);
        if (new_distance < current_distance)
        {
            current_distance = new_distance;
            out.InsertBackboneCell(temp.row, temp.col);
            selected_bb_cell = temp;
        }
    }
}


// move from p to one point of his neighborhood
point MakeRandomMove(point p) 
{  
    point p2;

    p2.row = p.row + Random (-1, 1);
    p2.col = p.col + Random (-1, 1);

    return p2;
}


unsigned SelectRouter(const RP_Input& in, RP_Output& out)
{
    point selected;
    unsigned size;

    selected = MaxDegreeRouter (in, out);
    
    if (IsFeasibleRouter(in, out, selected))
    {
        size = out.FindCoverageArea(selected).size();

        out.InsertRouter(selected.row, selected.col);
        cout << "Covered: " << out.TotalCoveredPoints() << endl;

        out.UpdateUncovered();

        ConnectRouterWithBackbone(out, in, selected);
        return size;
    }
    else
        return 0;
}


void GreedyRP(const RP_Input& in, RP_Output& out)
{ 
    unsigned new_coverage;

    do
    {    
        if (out.RemainingBudget() >= int(in.RouterPrice()))
            new_coverage = SelectRouter(in, out);
        else
        {
            cout << "No router placed." << endl;
            new_coverage = 0;
        }
    } while(new_coverage > 0);

    cout << out;
    cout << "Final Score: " << out.ComputeTotalScore() << endl;
    cout << "Remaining budget: " << out.RemainingBudget() << endl;
    // cout << "New Backbone:" << endl;
    // PrintVec(out.Backbone());
}

point MaxDegreeRouter (const RP_Input& in, RP_Output& out) 
{
    int max_degree = 0;
    int router_degree;
    point max;

    for (int i = 0; i < out.UncoveredSize(); i++)
    {             
        router_degree = out.RouterScore(out.Uncovered(i));

        if (router_degree > max_degree)
        {
            max_degree = router_degree;
            max.row = out.Uncovered(i).row;
            max.col = out.Uncovered(i).col;
        }
    }
    return max;
}

