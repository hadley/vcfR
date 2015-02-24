// [[Rcpp::plugins(cpp11)]]
#include <Rcpp.h>
#include <string>

using namespace Rcpp;


int elementNumber(String x, std::string element = "DP"){
  //
  //  Determine the position of a query element
  //  in a colon delimited string.
  //
  //  x is a string similar to:
  //  GT:GQ:DP:RO:QR:AO:QA:GL
  //
  //  element is which element 
  //  we're looking for.

//  int eNum = 0;
  int start = 0;
//  int len = 0;
  int pos = 1;
  int i;
  std::string istring = x;
  istring = istring + ":";
  std::string teststring;

  // Scroll through string.
  // Every time we find a delimiter (:)
  // check to see if we have our field.
  for(i=1; i <= istring.size(); i++){
    if(istring[i] == ':'){
      teststring = istring.substr(start, i-start);
      if(teststring == element){
        return pos;
      } else {
        pos = pos + 1;
        start = i+1;
        i++;
      }
    }
  }
  // If we get here then we did not observe our element.
  return 0;
}


// Don't really need this here.
// Perhaps for CharacterMatrix function?
std::string extractElementS(String x, int number=0){
  //
  // x is a colon delimited string similar to:
  // GT:GQ:DP:RO:QR:AO:QA:GL
  //
  // number is the position in the colon delimited 
  // string which needs to be extracted.
  //
//  int count = 0;
  int start = 0;
  int pos = 1;
  std::string istring = x;
  istring.push_back(':');
  std::string teststring;
  int i;

//  Rcout << "\nistring is: " << istring << " position is: " << number << "\n";
//  Rcout << "x is: ";
//  Rcout << istring;
//  Rcout << "\n";


  for(i=1; i <= istring.size(); i++){
    if(istring[i] == ':'){
//      Rcout << "Pos: " << pos << ",\tNumber: " << number << "\n";
      if(pos == number){
        teststring = istring.substr(start, i-start);
        return teststring;
      } else {
        start = i+1;
        pos++;
        i++;
      }
    }
  }
  // If we get here we did not find the element.
//  return istring;
  return std::string("NA");
//  std::string ostring = istring.substr(3,2);
//  return "yup\n";
//  return ostring;
}


double extractElementD(String x, int number=1){
  //
  // x is a string similar to:
  // GT:GQ:DP:RO:QR:AO:QA:GL
  //
  // number is the position in the colon delimited 
  // string which needs to be extracted.
  //
//  int count = 0;
  int start = 0;
  int pos = 1;
  std::string istring = x;
  std::string teststring;
  
  for(int i=1; i <= istring.size(); i++){
    if(istring[i] == ':'){
      if(pos == number){
        teststring = istring.substr(start, i-start);
        double teststring2 = atof(teststring.c_str());
        return teststring2;
//        return std::stod(teststring);
      } else {
        start = i+1;
        pos++;
        i++;
      }
    }
  }
  // If we get here we did not find the element.
  return(0);
}


//' Extract numeric data from genotype field of VCF
//' 
//' @param x A dataframe
//' @param element A string matching the element to be retrieved, for example 'DP'
//' @export
// [[Rcpp::export]]
NumericMatrix extractGT2NM(DataFrame x, std::string element="DP") {
  int i = 0;
  int j = 0;

  NumericMatrix outM(x.nrows(), x.size()-1);
  // Vector to check out DataFrame columns to
//  StringVector format = x(0);
  StringVector column = x(0);
  
  // Vector to hold position data
//  std::vector<int> positions(format.size());
  std::vector<int> positions(column.size());
  //
//  Rcpp::NumericVector outV(format.size());
  
  
  // Determine the position where the query element is 
  // located in each row (varioant)
//  for(i=0; i<format.size(); i++){
  for(i=0; i<column.size(); i++){
//    positions[i] = elementNumber(format(i), element);
    positions[i] = elementNumber(column(i), element);
  }


  for(i=1; i<x.size(); i++){ // Sample (column) counter
//    StringVector sample = x(i);
    column = x(i);
//    for(j=0; j<format.size(); j++){ // Variant (row) counter
    for(j=0; j<column.size(); j++){ // Variant (row) counter
//      outM(j, i-1) = extractElementD(sample(j), positions[j]);
      outM(j, i-1) = extractElementD(column(j), positions[j]);
    }
  }

  return outM;
}



// [[Rcpp::export]]
DataFrame extract_GT_to_DF(DataFrame x, std::string element="DP") {
  int i = 0;
  int j = 0;
//  Rcpp::DataFrame outDF(x.size()); // DataFrame for output
  Rcpp::DataFrame outDF = clone(x); // DataFrame for output
  
  Rcpp::StringVector column = x(0);   // Vector to check out DataFrame columns to
  std::vector<int> positions(column.size());  // Vector to hold position data

  // Determine the position where the query element is 
  // located in each row (varioant)
  for(i=0; i<column.size(); i++){
    positions[i] = elementNumber(column(i), element);
    column(i) = element;
  }
  outDF(0) = column;
  
  for(i = 1; i < x.size(); i++){ // Sample (column) counter
    column = x(i);
    for(j=0; j<column.size(); j++){ // Variant (row) counter
      column(j) = extractElementS(column(j), positions[j]);
    }
    outDF(i) = column;
  }
  return outDF;
}


// [[Rcpp::export]]
CharacterMatrix extract_GT_to_CM(DataFrame x, std::string element="DP") {
  int i = 0;
  int j = 0;
  Rcpp::StringVector column = x(0);   // Vector to check out DataFrame columns to
  std::vector<int> positions(column.size());  // Vector to hold position data
  Rcpp::CharacterMatrix cm(column.size(), x.size() - 1);  // CharacterMatrix for output
  
  // Swap column names, minus the first, from x to cm
  Rcpp::StringVector colnames = x.attr("names");
  colnames.erase(0);
  cm.attr("dimnames") = Rcpp::List::create(Rcpp::CharacterVector::create(), colnames);
  
  // Determine the position where the query element is 
  // located in each row (varioant)
  for(i=0; i<column.size(); i++){
    positions[i] = elementNumber(column(i), element);
  }
  
  // Process the input DataFrame
  for(i = 1; i < x.size(); i++){ // Sample (column) counter
    column = x(i);
    for(j=0; j<column.size(); j++){ // Variant (row) counter
      cm(j, i-1) = extractElementS(column(j), positions[j]);
    }
  }

  return cm;
}


// [[Rcpp::export]]
NumericMatrix CM_to_NM(CharacterMatrix x) {
  int i = 0;
  int j = 0;
  Rcpp::NumericMatrix nm(x.nrow(), x.ncol());  // NumericMatrix for output
  nm.attr("dimnames") = x.attr("dimnames");

  for(i=0; i<x.ncol(); i++){
    for(j=0; j<x.nrow(); j++){
      nm(j, i) = atof(x(j, i));
    }
  }

  return nm;
}