= テスト用のクラス

本章では、Railsが提供しているテスト用のクラスについて説明します。

== クラスの一覧

Railsでは、テスト用のクラスとして、次のようなクラスを提供しています。

//table[test_classes][Railsが提供しているテスト用のクラス]{
クラス名
-------------------------------------------------------------
ActiveSupport::TestCase
ActionMailer::TestCase
ActionView::TestCase
ActionController::TestCase
ActionDispatch::IntegrationTest
ActionDispatch::SystemTestCase
ActiveJob::TestCase
ActionCable::TestCase
ActionCable::Channel::TestCase
ActionMailbox::TestCase
Rails::Generators::TestCase
//}

名前から何となく何の為のクラスが想像出来るのではないかと思います。基本的には各ライブラリ毎にクラスが提供されているので、そのライブラリを使用した機能のテストをしたい場合、それぞれ向けに提供されているテストを使えば良いようになっています。

なお、Active RecordやActive Modelのような一部ライブラリでは専用のクラスは提供されていません。これはActiveSupport::TestCaseを使用すれば十分で、特別な処理を提供する必要が無い、と判断されている為です。専用のクラスが無い場合は、ActiveSupport::TestCaseを使用するようにして下さい。

== ActiveSupport::TestCase

前章でも述べた通り、Railsが提供しているテストクラスのベースになっているクラスです。各テストクラス共通で使用したい機能があった場合、このクラスに機能を追加する、このクラスでmoduleをincludeする、等を行うようになっています。

例えば、@<code>{assert_not}等の@<code>{assert_not_x}で始まるメソッドはActiveSupport::TestCaseに定義されており、全てのテスト用のクラスで使用出来るよになっています。他にも、ActiveSupport::Testing::TimeHelpersという時間を処理する為の@<code>{travel}、@<code>{travel_to}等のメソッドが定義されているmoduleがincludeされています。

//list[travel_to][travel_to]{
Time.current # => Wed, 27 Mar 2019 11:56:31 JST +09:00

# 1日前の移動
travel 1.day do
  Time.current # => Tue, 26 Mar 2019 11:56:35 JST +09:00
end

Time.current # => Wed, 27 Mar 2019 11:56:38 JST +09:00
//}

また、ActiveSupport::TestCaseはRails自体のテストでも使用されています。そのため、Rails自体のテストをする為に必要なメソッドもこのクラスで使用出来るようになっています。

例えば、何か既存の機能をdeprecateにしたい場合に、その機能がdeprecateになっている事を確認する為の@<code>{assert_deprecated}というアサーションがあります。これはRails全てのライブラリで使用したいアサーションなので、ActiveSupport::TestCaseで必要なmoduleがincldueされるようになっています。

//list[assert_deprecated][assert_deprecated]{
# update_attributesメソッドを使用するとdeprecateメッセージが出る
topic = Topic.find(1)
assert_deprecated do
  topic.update_attributes("title" => "The First Topic Updated")
end
//}

クラスやメソッドについての詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActiveSupport/TestCase.html} を参照してください。

== ActionMailer::TestCase

メール(送信)のテストの為のクラスです。当然のことですが、テストで実際にメールを送信するわけにはいきません。ActionMailer::TestCaseでは、メール送信処理が実行されても実際のメールの送信は行わず、代わりに送信処理が実行されたメールを配列で管理するようにしています。合わせて、送信処理が呼ばれた(または呼ばれていない)事を確認する為のアサーションを提供しています。

//list[assert_emails][assert_emails]{
test "invite friend" do
  # invite_friend_urlにPOSTしたら招待用のメールが送信される
  assert_emails 1 do
    post invite_friend_url, params: { email: 'friend@example.com' }
  end
end
//}

@<code>{assert_emails}は同期処理(@code{deliver_now})、非同期処理(@code{deliver_later})どちらで送信されたメールもチェックの対象になります。非同期で送信されたメールだけチェックしたい場合は、@<code>{assert_enqueued_emails}を使用する必要があります。

アサーションについての詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActionMailer/TestHelper.html} を参照してください。

== ActiveJob::TestCase

ジョブのテストの為のクラスです。メールと事なり、ジョブはテストで実際に実行しても問題無い事が多いでしょう。しかし例えば、「1時間後に実行されるジョブ」があった場合、テストで実際に1時間待つ訳にはいきません。ActiveJob::TestCaseでは、ジョブの登録処理が行われたらそのジョブを内部で保持し、どのようなジョブが登録されたかを確認出来るようにしています。当然、その登録された内容を確認する為のアサーションも提供されています。

//list[assert_enqueued_jobs][assert_enqueued_jobs]{
test "withdrawal" do
  user = User.last
  # ユーザが退会したらLoggingJobが登録される
  assert_enqueued_with(job: LoggingJob) do
    user.withdrawal
  end
end
//}

どのような引数が指定されたかチェックしたり、登録されたジョブを実行したりする事も出来るようになっています。アサーションについての詳細は、@<href>{https://edgeapi.rubyonrails.org/classes/ActiveJob/TestHelper.html} を参照してください。

== ActionView::TestCase

名前からAction Viewのテンプレートに関するテスト用のクラスかと推測されるかと思うのですが、実際はちょっと異なりviewのhelperのテスト用のクラスです。ActionView::TestCaseではhelerを使用する為に必要なcontrollerやviewの生成処理を行ってくれます。

//list[action_view_test_case][ActionView::TestCase]{
module UsersHelper
  def link_to_user(user)
    link_to "#{user.first_name} #{user.last_name}", user
  end
end

class UsersHelperTest < ActionView::TestCase
  test "link_to_user returns link with user name" do
    user = User.find_by(first_name: "Yamada", last_name: "Taro")
    assert_dom_equal %{<a href="/user/#{user.id}">Yamada Taro</a>},
      link_to_user(user)
  end
end
//}

しかし、Rails 4.2よりhelperのテストはそもそも生成されなくなり@<fn>{helper}、このクラスが使用される事は無くなりました。また、helper単体でのテストはあまり意味が無い@<fn>{helper_test}のでは、という声もあり、このクラスを使用する事は基本的に無いかと思います。
//footnote[helper][@<href>{https://github.com/rails/rails/commit/a34b6649d061977026db7124d834faccdf5bd8ef}]
//footnote[helper_test][IntegrationテストやSystemテストビューのテストと合わせてやるのが良いのでは、という意見が多いです。]

== ActionController::TestCase

コントローラーのテストの為のクラスです。特定のコントローラーのメソッドに対して、HTTPリクエストの送信及びレスポンスの確認ができるようになっています。

//list[action_controller_test_case][ActionController::TestCase]{
test "should get index" do
  get :index
  assert_response :success
end

test "should create user" do
  assert_difference('User.count') do
    post(:create, params: { user: { email: @user.email, name: @user.name } })
  end

  assert_redirected_to user_url(User.last)
end
//}

しかし同様にコントローラーのテストを行う為のクラスとしてActionDispatch::IntegrationTestがあります。ActionDispatch::IntegrationTestだとルーティングもセットでテストが出来る@<fn>{routing}、HTTPリクエストがより実際のリクエストに近い形で送信される等のメリットがあるのですが、実行はActionController::TestCaseの方が高速だった為、コントローラーのテストには長らくActionController::TestCaseが使われるようになっていました。
//footnote[routing][ActionController::TestCaseは送信先にコントローラーのアクション名を指定する為、ルーティングのテストは出来なかったのでした。]

しかし、Rails 5.0でActionDispatch::IntegrationTestのパフォーマンスが大幅に改善され、実行速度の差は大分縮まりました。結果、コントローラーのテストでもActionDispatch::IntegrationTestが使用される事が推奨されるようになり、scaffoldで生成するコントローラーのテストでもActionDispatch::IntegrationTestが使用されるようになりました。

なお、その際にActionController::TestCaseはgemに切り出してRails本体から削除する、という話があったのですが、何だかんだまだコードは残ったままになっています。とはいえ、機能追加等が行われる事は(恐らく)無いので、新規に追加するテストについてはActionDispatch::IntegrationTestを使用する事をおすすめします。

== ActionDispatch::IntegrationTest
== ActionDispatch::SystemTestCase
== ActionCable::TestCase
== ActionCable::Channel::TestCase
== ActionMailbox::TestCase
== Rails::Generators::TestCase
