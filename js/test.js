import {Ice} from 'ice';

let communicator;
try
{
    communicator = Ice.initialize(process.argv);
    const hello = await Demo.HelloPrx.checkedCast(
        communicator.stringToProxy("hello:tcp -h localhost -p 10000"));
    await hello.sayHello();
}
catch(ex)
{
    console.log(ex.toString());
}
finally
{
   if(communicator)
   {
      await communicator.destroy();
   }
}