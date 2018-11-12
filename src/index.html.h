#ifndef Index_h
#define Index_h

#include <stdint.h>

uint8_t html[] = "<html ng-app=\"app\">"
 "<head><title>Solarino</title>" 
 "<script src=\"https://ajax.googleapis.com/ajax/libs/angularjs/1.7.5/angular.min.js\"></script>"
 "<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/3.3.7/css/bootstrap.min.css\">"
 "<script>"
 "angular.module(\"app\", []);" 
 "angular.module(\"app\").controller(\"appCtrl\", function ($scope, $interval, $http) {" 
 "$scope.message = \"Solarino.\";"
 "var auto = $interval(function() {"
	"$http.get(\"http://192.168.0.100:80/values\").then(function(response){"
 		"$scope.values = response.data;"
 	"});"
 "}, 5000);" 
 "});" 
 "</script>" 
 "</head>" 
 "<body>" 
 "<div ng-controller=\"appCtrl\">"
 "{{message}} {{values}}"
 "</div>" 
 "</body>" 
 "</html>";

 #endif