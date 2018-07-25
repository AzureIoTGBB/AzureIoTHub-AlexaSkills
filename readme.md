﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿﻿Under Construction- - - # How to Get Azure IoT Hub to work with Amazon Alexa SkillsThis guide is the result of a project that we worked with a customer on.  The customer wanted to connect an Amazon Alexa skill to Azure IoT Hub to perform some "action".  The short answer here is that you can achieve this...but the longer response is the "how".  We are creating this to address the "how" and we hope that you enjoy the article.#### <u> Assumptions</u>To start out we need to make a few assumptions: 1. That you have access to an Azure Subscription 2. That you have configured an Azure IoT Hub in the Azure Subscription- - - ## Configure the Amazon Alexa Service (Skills)The first step here is to log into the [Azure Development portal](https://developer.amazon.com/) ![image](Pictures/1.png) By clicking on Alexa skill kit, you start this process.Then you will need to click on "Start a Skill" ![image](Pictures/2.png) Click on "Create Skill" ![image](Pictures/3.png) Now you should see the following page.  Here you will give your skill a name and select the default language. ![image](Pictures/4.png)You will then have a selection of different skill types.  For this tutorial we will select "Custom" ![image](Pictures/5.png)After you select the custom skill click on the "Create Skill" button.This will take us to the Alexa Skill build page where we can start building out our Alexa Skill. ![image](Pictures/6.png) The invocation name is how the user will call the skill.  Our next step is to define this invocation name.  Keep in mind that this is how the user will call up the Alexa Skill.  For example you are building a Skill for measuring the amount of moisture in soil, you might want your users to ask Alexa something like "Alexa ask my soil how moist it is."  In this case the invocation name would be "my soil".Click on the invocation on the left hand side of the skill builder. ![image](Pictures/7a.png) From here enter your invocation name Always remember to save your skill model! ![image](Pictures/8.png) Next on the list is to define the skill intent.  An intent represents an action that fulfills a user's spoken request.  On the left hand side click "Intents" (There are 4 default intents) and then click "Add". ![image](Pictures/9a.png) On the right click the "Add Intent" button.   ![image](Pictures/9b.png) For our purposes we will be turning on and off LED's so our intent will be called "turnlightonoff". ![image](Pictures/9.png) Click on "Create custom intent" ![image](Pictures/9c.png) We now need to define the utterances.  An utterance is spoken phrase that activates the intent.  There could be several different ways that a user could ask to turn on or off the LED's.  For the purposes of this document we will just use "turn light" followed by an "Slot".  Once this is filled out click on the "+" at the end of the row.  This would look like the following: ![image](Pictures/10.png)Now that we have defined our utterances, we need to create a custom slot type.  To do this we need to click on "Slot Types" on the left hand side and then click the "Add" button. ![image](Pictures/11a.png) Slot types are kind of a "fill in the blank" value.  In our example above we are using "{actions}".  So when creating out slot type we will call it "action".   ![image](Pictures/11.png) This "action" will have two values (mostly because we want to turn on the LED and turn off the LED).  Enter ON in the "Enter a new value for this slot" box and press the "+" at the end of the row.  Create an Off in the same manner.  In the picture below you can see the slot type "action" set with two values of ON and OFF.  This also matches our Arduino functions...you will see that later when we get to that section.  In the "Create custom slot type" type action.  ![image](Pictures/12.png) Moving along with the build, we now need to define our endpoint.  Since we will be using Azure IoT Hub to control the LED's we will need to navigate to the "Endpoints" on the left hand side. ![image](Pictures/13a.png) From here we will select the HTTPS endpoint. ![image](Pictures/13.png) Now let this window sit for a second and open a new window and log into your Azure Portal. This is where it will start to get a little interesting.  We need the Endpoint for our Azure Function App.  From the Azure portal click on the green + "Create a Resource".  In the search field type in "function app".   ![image](Pictures/14.png) Click on the "Function App" ![image](Pictures/15.png) Click "Create" ![image](Pictures/16.png) Fill in the values for the Function App ![image](Pictures/17.png) Click "Create"It will take a few moments for the function app to spin up.  Once it completes navigate to the function app and click on the "+" sign next to "functions" as shown blow. ![image](Pictures/18.png) Click on "Custom Functions: ![image(Pictures/19.png) Select HTTP Trigger ![image(Pictures/20.png)On the next blade select C# for the language, git it a name, and leave the authorization level as function. ![image(Pictures/22.png) Click "Create"We will come back later to fill in the function, but for now we just need the URL to the function.  From here we can click on "</> Get Function URL" and this will open up a new window as seen below. ![image(Pictures/23.png) Click "Copy" to the right of the URL. ![image(Pictures/25.png) Now that we have the URL to the Azure function we can head back to the Amazon Alexa skill builder portal.  We should be looking at the Endpoints page as we left it...and this is where we will pick back up.  Enter the URL value for the Azure Function in the URL section for the HTTPS URL. ![image(Pictures/26.png) Once you have entered your Azure Function URL the next step is to select the SSL certificate type.  For this example we will be using the "My development endpoint is a sub-domain of a domain that has a wildcard certificate from a certificate authority" option ![image(Pictures/27.png) Phew!!  Time to take a break.  Now that you are back from your break, let's continue.  From there the Alexa Skill is setup to talk to Azure, but we need to prepare the Azure side of equation.  - - - ## Configure Azure- - - If you are signed out of the Azure portal, sign back in.  Now navigate back to the Azure Function App that we stood up from the steps from above.![image](Pictures/28.png)Over on the left hand side you will see a ![image](Pictures/28b.png) as seen below.  Click on that ![image](Pictures/28b.png) image and that will open up the files fly out.![image](Pictures/28.png)Under the "View files" section click the "+Add" button.![image](Pictures/29.png)In the file name box enter "project.json" without the quotes and press enter.![image](Pictures/30.png)In the code section enter the following:```C#{	"frameworks":{     "net46":{      "Dependencies":      {"Microsoft.Azure.Devices":"1.6.0"      }     }    }}```![image](Pictures/31.png)And click Save.Click on the run.csx file.![image](Pictures/32.png)Replace all this code with the following:<i>Note that this code is also found in the code directory under the file called Azure_Run_Function.</i>![image](Pictures/33.png)```C#using System;using Microsoft.Azure.Devices;  // need version 1.4 else the Async will not replyusing System.Net;using System.Threading;using System.Net.Http;using Newtonsoft.Json;using Newtonsoft.Json.Linq;const string IoTConnectionString = "<IoT Hub connection String>";const string device = "<IoT Device Name>";static string responseText = "";static string intentAction = "";static ServiceClient serviceClient; public static async Task<HttpResponseMessage> Run(HttpRequestMessage req, TraceWriter log){    dynamic data = await req.Content.ReadAsAsync<object>();    log.Info($"Content={data}");    // Set name to query string or body data    string intentName = data.request.intent.name;    intentAction = data.request.intent.slots.action.value;    log.Info($"intentName={intentName}");    switch (intentName)    {        case "turnlightonoff":            if (intentAction == "on")            {                 responseText = "Sure thing Keith.  Let me trun on the light for you.";            }            else if (intentAction == "off")            {                 responseText = "Sure thing Keith.  Let me turn off the light for you.";             }            else             {                responseText = "I am not sure of what just happened.";            }        break;        default:             responseText = "Keith I am unsure of your intent.";            break;    }        light(log);                return req.CreateResponse(HttpStatusCode.OK, new {            version = "1.1",            sessionAttributes = new { },            response = new                {                    outputSpeech = new                    {                        type = "PlainText",                        text = responseText                    },                    shouldEndSession = true                }            });         }public static async void light(TraceWriter log){        serviceClient = ServiceClient.CreateFromConnectionString(IoTConnectionString);        var commandMessage = new Message(System.Text.Encoding.ASCII.GetBytes(intentAction.ToUpper()));        // send the command to the device        await serviceClient.SendAsync(device, commandMessage);  }```And then click save.Now the Azure Function is setup for the incoming messages from the Alexa skill.  ## Connecting our device to the Auzre IoT Hub- - - We will need to create a virtual device in the IOT Hub for the physical device to connect to.  Once again...let's go back in the Azure portal and navigate to the resource group that we are using for this project. Select the IoT Hub.  Once the IoT Hub blade opens up navigate to the click on IoT Devices on the left hand side. ![image](Pictures/34.png)Click on the "+Add" button.![image](Pictures/35.png)Fill in the device name and click save.![image](Pictures/36.png)After the device is added to the IoT Hub we need to generate a SAS Token for the device.  One of the easiest ways to generate a SAS token for the device is to install the IoT Explorer.  You an download the IoT Explorer from [here](https://github.com/Azure/azure-iot-sdks/releases/download/2016-11-17/SetupDeviceExplorer.msi).  While that is downloading, let's get the IoT Hub Connection string.  To get the IoT Hub Connection string log into the Azure portal.  Navigate to the IoT Hub.  On the left hand side click on the click on the "Shared access policies". ![image](Pictures/37.png)Once the flyout opens click on ![image](Pictures/40.png) next to the Connection string—primary key.  This will copy it to clipboard.  Now back to the Device Explorer. ![image](Pictures/38.png)Click on the IoT Hub Owner ![image](Pictures/39.png)Once you have the device explorer installed start it up and enter the Connection string—primary key that you have on the clipboard in the box as scene below ![image](Pictures/41.png)Click on update and then click on "Generate SAS token" ![image](Pictures/42.png)Copy this SAS Token and keep it on the clip board for the device configuration in the next section.- - - ## Programming the device to connect to the IoT Hub- - - Open the Arduino_Code_for_device.ino from the code directory of this repo and we need to make a few changes.```Cconst char* ssid      = "<SSID>";const char* password  = "<Password for WIFI Access>";const char* mqtt_server = "<IoT Hub URL>";const char* deviceName  = "<IoT Device Name>";const char* deviceSAS   = "<IoT SAS token>"; ```So from the above section:Replace the <SSID> with the SSID that you will be connecting the device to.Replace the <Passoword for WIFI Access> with the Password for the SSIDReplace the <IoT Hub URL> with the IoT Hub URL from the Azure Portal* Navigate to the Azure Portal , select the resource group that this project is in and then select the IoT Hub.  ![image](Pictures/47.png)* Copy the hostname as shown above.Replace the <IoT Device Name> with the device name you created earlier in this tutorial.Replace the <IoT SAS token> with the SAS token that is on clipboard (if it is not on the clipboard you can get it again in Device Explorer as shown above)we are now ready to publish the Arduino_Code_for_device.ino file to the device.  But first we need to connect the LED to the device.  Below is how to connect the LED to the ESP8266 development board. ![image](Pictures/AlexaToAzure_bb.png)```AssumptionAssumption:  You have the ESP8266 board manager configured in the Arduino IDE.  If you do not [see this article](http://randomnerdtutorials.com/how-to-install-esp8266-board-arduino-ide/)```Connect the ESP8266 to the USB port and push the configuration to the ESP8266. ![image](Pictures/48.png)Your device should now connect to the IoT Hub.  You can verify this by navigating to the IoT Hub in Azure and selecting IoT Devices from the left hand side and then finding the device on the right pane under Device ID: ![image(Pictures/49.png)- - - ## Now back to the Alexa Skill page- - -Now that we have the device code deployed, the Azure Function code configured, the Device connected to the IoT Hub it is time to test the configuration end to end.From the Alexa skill page select "Test" ![image](Pictures/50.png)From here you can enter type in the command or you can talk to the computer to test the skill: ![image](Pictures/51.png)If all things work well...you should get a response back as seen below.  This response is coming from the Azure function that we configured earlier.I hope that you enjoyed this tutorial and walk though...          